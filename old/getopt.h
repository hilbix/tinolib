/* $Header$
 *
 * NOT REALLY READY YET, it's as far as I need it.
 * YES, AGAIN, IT IS REALLY NOT READY YET!
 *
 * My way of doing getopt:
 *
 * This differs from ordinary getopt in several ways:
 * - It forces you to have a version.
 * - It implicitely prints the usage, too.
 * - It is easy to use and easy to understand (not cryptic).
 * - Parses parameters from the function call.
 * - It does not handle long and short options differently.
 * - It will not parse options everywhere in the command line.
 * - It is easy to extend (at least for me).
 * - No need for global variables or such.
 *
 * Sideeffects:
 * - If posixly correct then it must "shuffle" the argv array.
 *   (Currently posixly correct is not supported anyway.)
 * - User can "hack" the options easily using a HEX editor.
 *
 * $Log$
 * Revision 1.1  2004-09-30 22:15:12  tino
 * don't want to loose this shity intermediate code ;)
 *
 */

#ifndef tino_INC_getopt_h
#define tino_INC_getopt_h

/* Always start the global arg to GETOPT with this string!
 */
#define TINO_GETOPT_DEFAULT(VERSION)	VERSION "\t" __DATE__ "\t"

#define TINO_GETOPT_DEBUG	"debug "	/* prefix to debug	*/

/* Global flags, just concatenate them like:
 * TINO_GETOPT_DEFAULT TINO_GETOPT_TAR
 *
 * Follow this with a TAB and the global command line usage string
 */
#define TINO_GETOPT_TAR		"tar "	/* first arg is options like in TAR */
#define TINO_GETOPT_POSIX	"posix "/* search all args for options	*/
#define TINO_GETOPT_PLUS	"plus "	/* allow +option, too (invert flags) */
#define TINO_GETOPT_LONG	"l "	/* allow long options with -	*/
#define TINO_GETOPT_LONG_LONG	"ll "	/* parse long options with --	*/

/* Options to data types.
 *
 * If this is present, it alters the behavior
 * and/or fetches some additional data.
 */

/* This is the "usage" option, print usage
 */
#define TINO_GETOPT_USAGE	"usage "

/* Fetch a user pointer.
 * Usually used in global.  Locally it overwrites only on one time.
 *
 * The user pointer is passed to parser functions.
 *
 * Fetches arg:
 *	void *
 */
#define TINO_GETOPT_USER	"user "

/* Fetch argument processing function.
 * Usually used in global.  Locally it overwrites only on one time.
 *
 * The function will get:
 *	The pointer of the argument to set, already set to the value.
 *	The pointer to the argv[] index.
 *	The pointer to the option string.
 *	The user pointer.
 * The function must return:
 * NULL	to accept the value (probably after altering it)
 * ""	(empty string) to reject the value (print usage)
 * str	(nonempty string) abort getopt and print the str as explaination
 *
 * Fetches arg:
 *	const char *fn(void *ptr, const char *arg, const char *opt, void *usr);
 */
#define TINO_GETOPT_FN		"fn "


/* Data type of options.
 * Give them and concatenate options if needed.
 * These are mutually exclusive
 * (the last one wins, but don't depend on this).
 *
 * Follow this by a TAB, the option string, a TAB, the usage string
 */

/* A flag taking no argument.
 *
 * Needs a pointer to:
 *	int
 * If option present:
 *	The integer will be set to 1
 * else:
 *	Stores NULL
 */
#define TINO_GETOPT_FLAG	"f "

/* A string argument.
 *
 * Needs a pointer to:
 * 	const char *
 * If option present:
 *	Stores the argument into the const char *
 * else:
 *	Stores NULL
 */
#define TINO_GETOPT_STRING	"s "	/* argument with string	*/

/* A "help" option.
 *
 * When this option is present, the given string (the pointer)
 * is printed out, the program is not started!
 *
 * Needs a pointer to:
 * 	char
 */
#define TINO_GETOPT_HELP	"help "

/* Return TRUE if the prefix is part of the arg.
 * Do you really need getopt to be efficient?
 *
 * Arguments are of the form
 *	prefix prefix prefix option TAB help
 * where prefix always (are separated with/)ends in a SPC
 *
 * Not very thrilling, indeed.
 */
static int
tino_getopt_prefix(const char *arg, const char *prefix)
{
  /* What a whacky code!
   * It's thought to be good for the optimizer.
   */
  for (;; arg++)
    {
      const char	*p;

      /* Are we at an end?
       */
      switch (*arg)
	{
	case ' ':
	  continue;

	case '\t':
	case 0:
	  return 0;

	default:
	  break;
	}

      /* Do we have a prefix match?
       * Note that *arg cannot be 0 here at entry.
       */
      for (p=prefix; *arg++==*p; )
	if (!*++p)
	  return 1;

      /* Nope, hunt for the next -
       * (Hopefully the optimizer can sort out this offset calcs.)
       */
      arg	-= 2;
      for (;;)
	{
	  switch (*++arg)
	    {
	    default:
	      continue;

	    case 0:
	    case '\t':
	      return 0;

	    case ' ':
	      break;
	    }
	  break;
	}
    }
}

static int
tino_getopt_help(const char *arg)
{
  for (;; arg++)
    {
      switch (*arg)
	{
	case ' ':	/* SPC might get another meaning in future	*/
	case '\t':
	  return arg;

	case 0:
	  return 0;
	}
    }
}

/* Process the arg.
 * Returns NULL if end of prefixes.
 */
#define	IFgen(X,Y)							\
  if (!strncmp(arg, TINO_GETOPT_##X, (sizeof TINO_GETOPT_##X)-1))	\
    {									\
      if (p->debug)
        fprintf(stderr, "getopt: " #X "\n");
      Y;								\
      arg+=sizeof(TINO_GETOPT_##X)-1;					\
    }									\
  else

#define IFarg(X,Y) do { if ((p->(X)=va_arg(*list,(Y)))==0) return 0; } while(0)
#define IFflg(X)   IFgen(X,p->(X)=1)
#define IFptr(X,Y) IFgen(X,IFarg(X,Y))
#define IFtyp(X)   IFgen(X,p->type=TINO_GETOPT_##X)

static struct tino_getopt_impl *
tino_getopt_arg(struct tino_getopt_impl *p, va_list &list)
{
  const char	*arg;

  p->opt	= 0;
  p->help	= 0;
  p->unknown	= 0;
  p->type	= 0;
  p->arg	= arg	= va_arg(*list, const char *);
  if (!arg)
    return 0;

  for (;;)
    IFflg(DEBUG)
      IFflg(TAR)
      IFflg(POSIX)
      IFflg(PLUS)
      IFflg(L)
      IFflg(LL)
      IFflg(USAGE)
      IFtyp(HELP)
      IFptr(USER,void *)
      IFptr(FN,const char *(*)(void *, char **, const char *, void *))
      IFtyp(FLAG)
      IFtyp(STRING)
    {
      p->opt	= arg;
      for (;;)
	{
	  switch (*arg++)
	    {
	    default:
	      continue;

	    case '\t':
	      p->help	= arg;
	      if (p->debug)
		fprintf(stderr, "getopt help: '%s'\n", arg);
	    case 0:
	      if (p->debug)
		fprintf(stderr, "getopt option: '%*s'\n", arg-p->opt, p->opt-1);
	      if (p->type)
		IFarg(var,void *);
	      return p;

	    case ' ':
	      if (p->debug)
		fprintf(stderr, "getopt unknown: '%*s'\n", arg-p, p);
	      if (!p->unknown)
		p->unknown	= p->opt;
	      break;
	    }
	  break;
	}
    }
}
#undef IFtyp
#undef IFptr
#undef IFflg
#undef IFarg
#undef IFgen

/* returns:
 * - Offset in the arguments where the non-options start.
 * - or -1 on error/usage/etc.
 */
static int
tino_getopt(int &argc, char **argv,	/* argc,argv as in main	*/
	    int min, int max,		/* min..max args, max<min: unlimited */
	    const char *global		/* string of global settings	*/
	    /* append the general commandline usage to global (with a TAB) */
	    , ...
	    /* Now following "pairs" follow:
	     * A flag description string.
	     * optional FN or USER pointers as in description string.
	     * A pointer to the flag.
	     */
	    /* TERMINATE THIS WITH A NULL !!! */
	    )
{
  struct tino_getopt_impl	q = { 0 };
  const char			*arg;
  va_list			list;

  /* Print usage
   */
  fprintf(stderr, "Usage: %s %s\n", argv[0], global);

  va_start(list, global);
  while (tino_getopt_arg(&q, &list))
    fprintf(stderr, "\t%s\n", q->opt);
  va_end(list);
  return -1;	/* usage printed	*/
}
#endif
