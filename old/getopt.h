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
 * Revision 1.2  2004-10-10 12:48:39  tino
 * still not ready
 *
 * Revision 1.1  2004/09/30 22:15:12  tino
 * don't want to loose this shity intermediate code ;)
 *
 */

#ifndef tino_INC_getopt_h
#define tino_INC_getopt_h

#include <stdarg.h>

/* Always start the global arg to GETOPT with this string!
 */
#define TINO_GETOPT_DEFAULT(VERSION)	VERSION "\t" __DATE__ "\t"

#define TINO_GETOPT_DEBUG	"debug\1"	/* prefix to debug	*/

/* Global flags, just concatenate them like:
 * TINO_GETOPT_DEFAULT TINO_GETOPT_TAR
 *
 * Follow this with a TAB and the global command line usage string
 */
#define TINO_GETOPT_TAR		"tar\1"	/* first arg is options like in TAR */
#define TINO_GETOPT_POSIX	"posix\1"/* search all args for options	*/
#define TINO_GETOPT_PLUS	"plus\1"/* allow +option, too (invert flags) */
#define TINO_GETOPT_LONG	"l\1"	/* allow long options with -	*/
#define TINO_GETOPT_LONGLONG	"ll\1"	/* parse long options with --	*/

/* Options to data types.
 *
 * If this is present, it alters the behavior
 * and/or fetches some additional data.
 */

/* This is the "usage" option, print usage
 */
#define TINO_GETOPT_USAGE	"usage\1"

/* Fetch a user pointer.
 * Usually used in global.  Locally it overwrites only on one time.
 *
 * The user pointer is passed to parser functions.
 *
 * Fetches arg:
 *	void *
 */
#define TINO_GETOPT_USER	"user\1"

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
#define TINO_GETOPT_FN		"fn\1"


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
#define TINO_GETOPT_FLAG	"f\1"

/* A string argument.
 *
 * Needs a pointer to:
 * 	const char *
 * If option present:
 *	Stores the argument into the const char *
 * else:
 *	Stores NULL
 */
#define TINO_GETOPT_STRING	"s\1"	/* argument with string	*/

/* A "help" option.
 *
 * When this option is present, the given string (the pointer)
 * is printed out, the program is not started!
 *
 * Needs a pointer to:
 * 	char
 */
#define TINO_GETOPT_HELP	"help\1"


/**********************************************************************/
/**********************************************************************/

/* Process the arg.
 * Returns NULL if end of prefixes.
 */
#define	IFgen(X,Y)							\
  if (!strncmp(arg, TINO_GETOPT_##X, (sizeof TINO_GETOPT_##X)-1))	\
    {									\
      if (p->DEBUG)							\
        fprintf(stderr, "getopt: " #X "\n");				\
      Y;								\
      arg+=(sizeof TINO_GETOPT_##X)-1;					\
    }									\
  else

#define IFarg(X,Y) do { if (((p->X)=va_arg(*list,Y))==0) return 0; } while(0)
#define IFflg(X)   IFgen(X,(p->X)=1)
#define IFptr(X,Y) IFgen(X,IFarg(X,Y))
#define IFtyp(X)   IFgen(X,p->type=TINO_GETOPT_##X)

struct tino_getopt_impl
  {
    const char	*arg;
    void	*var;
    /* broken up
     */
    const char	*unknown;	/* pointer to the first unknown	*/
    const char	*type;		/* pointer to a static string	*/
    const char	*opt;		/* pointer to the option name	*/
    int		optlen;		/* length of the option name	*/
    const char	*help;		/* pointer to help string	*/
    /* flags
     */
    int		DEBUG, TAR, POSIX, PLUS, LONG, LONGLONG, USAGE;
    /* pointers
     */
    void	*USER;
    const char	*(*FN)(void *, char **, const char *, void *);
  };

static struct tino_getopt_impl *
tino_getopt_arg(struct tino_getopt_impl *p, va_list *list, const char *arg)
{
  if (!arg)
    arg	= va_arg(*list, const char *);
  p->unknown	= 0;
  p->help	= 0;
  p->opt	= 0;
  p->type	= 0;
  p->arg	= arg;
  if (!arg)
    return 0;

  for (;;)
    IFflg(DEBUG)
      IFflg(TAR)
      IFflg(POSIX)
      IFflg(PLUS)
      IFflg(LONG)
      IFflg(LONGLONG)
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
	    case ' ':
	      p->help	= arg-1;
	      if (p->DEBUG)
		fprintf(stderr, "getopt help: '%s'\n", arg);
	    case 0:
	      p->optlen	= arg-p->opt-1;
	      if (p->DEBUG)
		fprintf(stderr, "getopt option: '%.*s'\n", p->optlen, p->opt);
	      if (p->type)
		IFarg(var,void *);
	      return p;

	    case '\1':
	      if (p->DEBUG)
		fprintf(stderr, "getopt unknown: '%.*s'\n", arg-p->opt, p->opt);
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

static int
tino_getopt_tab(const char *ptr, const char **set)
{
  const char	*tmp;

  for (tmp=ptr;;)
    {
      switch (*tmp++)
	{
	case 0:
	  *set	= tmp-1;
	  return tmp-ptr-1;

	case '\t':
	  *set	= tmp;
	  return tmp-ptr-1;
	}
    }
}

/* returns:
 * - Offset in the arguments where the non-options start.
 * - or 0 on usage or other things which are no real error.
 * - or -1 on error.
 */
static int
tino_getopt(int *argc, char **argv,	/* argc,argv as in main	*/
	    int min, int max,		/* min..max args, max<min: unlimited */
	    const char *global		/* string of global settings	*/
	    /* append the general commandline usage to global (with a SPC) */
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
  va_list			list;
  int				verslen, complen;
  const char			*compiled, *opts;
   
  verslen	= tino_getopt_tab(global, &compiled);
  complen	= tino_getopt_tab(compiled, &opts);
  tino_getopt_arg(&q, &list, opts);
  opts		= q.help;

#if 0
  for (pos=1; pos<=argc; pos++)
    {
    }
  va_start(list, global);
  va_end(list);

  /* If number of arguments are ok, just return the offset.
   */
  if (argc-pos>min && (max<min || argc-pos<=max))
    return pos;
#endif
    
  /* Print usage
   */
  {
    fprintf(stderr,
	    "Usage: %s [options]%s\n"
	    "\t\tversion %.*s compiled %.*s\n"
	    "Options:\n"
	    , argv[0], opts,
	    verslen, global,
	    complen, compiled);

    va_start(list, global);
    while (tino_getopt_arg(&q, &list, NULL))
      {
	fputc('\t', stderr);
	if (q.LONGLONG)
	  fputc('-', stderr);
	fprintf(stderr, "-%s\n", q.opt);
      }
    va_end(list);
  }
  return 0;	/* usage printed	*/
}
#endif
