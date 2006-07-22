/* $Header$
 *
 * *IMPORTANT*: If you have trouble using it, try to set
 * TINO_GETOPT_DEBUG as seen in the example at the end!
 *
 * NOT REALLY READY YET, it's as far as I need it.
 * YES, AGAIN, IT IS REALLY NOT READY YET!
 * AND IT's VASTLY UNTESTED!
 *
 * My way of doing "the swiss army knife of" getopt:
 *
 * This differs from ordinary getopt in several ways:
 * - It forces you to have a version.
 * - It implicitely prints the usage, too.
 * - It is easy to use and easy to understand (not cryptic).
 * - Parses parameters from one single function call.
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
 * You need a good compiler capable of calling functions with hundreds
 * of parameters.  Not kidding.
 *
 * For an example, see below!
 *
 * Apparently you are not allowed to use \1 in your strings.  ;)
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.18  2006-07-22 23:47:44  tino
 * see ChangeLog (changes for mvatom)
 *
 * Revision 1.17  2006/07/22 16:57:06  tino
 * Minor cleanups. VALID_STR improved (still not working).
 *
 * Revision 1.16  2006/07/22 16:42:04  tino
 * no change (just spaces)
 *
 * Revision 1.15  2006/07/17 16:03:11  tino
 * minor changes
 *
 * Revision 1.14  2006/06/11 19:47:25  tino
 * See ChangeLog
 *
 * Revision 1.13  2006/04/11 21:39:51  tino
 * getopt.h ci of old DEFAULT/NODEFAULT changes for future history
 *
 * Revision 1.12  2006/04/11 21:06:04  tino
 * DEFAULT/NODEFAULT added and some minor bugfixes in getopt.h
 *
 * Revision 1.11  2006/02/11 14:36:11  tino
 * 000; is now TINO_XXX;
 *
 * Revision 1.10  2005/12/20 00:29:59  tino
 * Now getopt.h shall compile under SuSE 7.2, too.
 *
 * Revision 1.9  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.8  2005/09/10 12:31:39  tino
 * cosmetic checkin
 *
 * Revision 1.7  2005/08/02 04:44:41  tino
 * C++ changes
 *
 * Revision 1.6  2005/02/05 23:50:02  tino
 * getopt now knows about dd options type
 *
 * Revision 1.5  2005/01/26 10:48:25  tino
 * Had some siedeffect in the output of the unit test.
 *
 * Revision 1.4  2005/01/04 13:23:49  tino
 * see ChangeLog, mainly changes for "make test"
 *
 * Revision 1.3  2004/10/22 00:56:24  tino
 * Getopt working version (nearly untested)
 *
 * Revision 1.2  2004/10/10 12:48:39  tino
 * still not ready
 *
 * Revision 1.1  2004/09/30 22:15:12  tino
 * don't want to loose this shity intermediate code ;)
 */

#ifndef tino_INC_getopt_h
#define tino_INC_getopt_h

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef	TINO_XXX
#define	TINO_XXX
#endif

/* Always start the global arg to GETOPT with this string!
 */
#define TINO_GETOPT_VERSION(VERSION)	VERSION "\t" __DATE__ "\t"


/**********************************************************************/
/* Parsing options:
 */
#define TINO_GETOPT_DEBUG	"debug\1"	/* prefix to debug	*/

/* Global flags, just concatenate them like:
 * TINO_GETOPT_VERSION("vers") TINO_GETOPT_TAR ...
 *
 * Follow this with a TAB and the global command line usage string.
 *
 * Examples of the various forms:
 *
 * TAR "tar xCfz dir file args" but "tar -xCdir -ffile -z args"
 *
 * POSIX "tar args -x", without POSIX this is parsed as
 * "tar -- args -x", so -x becomes an argument.
 *
 * PLUS "cmp -s" (sets silent) vs. "cmp +s" (clears silent)
 * Note: + can be behind the option, so -s+ and --longopt+
 * Note that + is similar to -- not -, so +sv means --sv+.
 * Perhaps best this is combined with LOPT.
 *
 * LLOPT allows to write the form "--file=path" or "--file path".
 *
 * LOPT allows long options with - only, this is magic.  This can have
 * deadly abiguities, if you have non unique prefixes like "s" and
 * "str".  This is resolved in random order and might not be what you
 * want.
 *
 * DIRECT is "-parg" where arg can be empty. "-p -x" is then not
 * interpreted as "-p with argument -x", instead it is interpreted as
 * "-p with empty argument, next option is -x".  You can find this in
 * MySQL.  Note that this disallows the form "--long arg" form and
 * does not enable "--longarg" except in the case where the long
 * option ends on something neither letter nor number
 * (i. E. --long:arg).  This is convenient for the LOPT form.
 *
 * DD is what you know from dd.  It allows the form: "long=arg" and
 * "long arg" (set DIRECT to disable this).  You probably should set
 * this with min=0 and max=0, else there can be deadly ambiguities to
 * other program arguments.
 *
 * Note that you even can parse commandlines in a stateful manner:
 *	cat -crlf file1 +crlf file2
 * where cr2crlf etc. are options to how the file must be processed.
 * For this use TINO_GETOPT_CB, which will get passed file1 and file2,
 * the options are set as appropriate.
 *
 * If you found a regular argument processing which cannot be
 * formulated here (heuristical commandline parsers like that Fortran
 * compilers do do not count!) please drop me a note.  ;)
 */

#define TINO_GETOPT_TAR		"tar\1"	/* first arg is options like in TAR */
#define TINO_GETOPT_POSIX	"posix\1"/* search all args for options	*/
#define TINO_GETOPT_PLUS	"plus\1"/* allow +option, too (invert flags) */
#define TINO_GETOPT_LOPT	"lo\1"	/* allow long options with -	*/
#define TINO_GETOPT_LLOPT	"llo\1"	/* parse long options with --	*/
#define TINO_GETOPT_DIRECT	"d\1"	/* arg follows option directly	*/
#define TINO_GETOPT_DD		"dd\1"	/* dd like, no prefixes at all, always long */


/**********************************************************************/
/* Option flags
 */

/* Options to data types.
 *
 * If this is present, it alters the behavior
 * and/or fetches some additional data.
 */

/* This is the "usage" option flag, print usage
 *
 * Note: This is a flag.  If used with others than TINO_GETOPT_HELP
 * you have to check for fUSAGE yourselt.
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

/* Fetch non-option processing callback.
 *
 * If you set this, this gets called with everything tino_getop cannot
 * process as an option (which will make up an argument to the
 * program).
 *
 * The function gets the current position, the pointer to argv array,
 * the argc parameter and the global user pointer.  It must return:
 * 0	if the argument was not eaten away (left for the program).
 * >0	to skip the next n arguments in argv.
 * <0	on error (print usage).
 * It probably makes no sense to set this outside of global.
 * If POSIX is not set and 0 is returned, this ends tino_getopt.
 *
 * Fetches arg:
 *	int fn(int pos, const char **argv, int argc, void *usr);
 */
#define	TINO_GETOPT_CB		"cb\1"

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

/* With NODEFAULT, the variables will not be initialized (takes
 * precedence).  With DEFAULT a default value will be fetched AFTER
 * the variable.  The default depends from the size of the variable,
 * so beware of long long and pointers (use NULL, not 0)!
 *
 * IMPORTANT:  DEFAULT must be given AFTER the variable pointer,
 * else you will get segmentatin violation!
 * 
 */
#define	TINO_GETOPT_NODEFAULT	"keep\1"/* do not init variable	*/
#define	TINO_GETOPT_DEFAULT	"def\1"	/* give variable defaults */

/* Min and Max parameters.
 * Must be followed by a Numeric Type from above.
 * The PTR version uses an pointer arg which points to the data.
 */
#define	TINO_GETOPT_MIN		"min"
#define	TINO_GETOPT_MAX		"max"
#define	TINO_GETOPT_MIN_PTR	"MIN"
#define	TINO_GETOPT_MAX_PTR	"MAX"


/**********************************************************************/
/* Arguments
 */

/* A flag taking no argument.
 *
 * Needs a pointer to:
 *	int
 * If option present:
 *	The integer will be set to 1
 * else:
 *	The integer will be set to 0 (or the default)
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

/* Give valid data for strings:
 * This reads away a pointer to NUL terminated strings.
 * The list must be terminated by NUL NUL.
 * Thus to include the empty string "", do it as the first string:
 * TINO_GETOPT_VALID_STR TINO_GETOPT_STR, "\0val1\0val2\0", &str,
 */
#define	TINO_GETOPT_VALID_STR	"val\1"

/* Byte flags, eat just one character.  Shoot me, but "-c1x" does
 * *not* mean "-c1 -x", it just ignores the x.
 */
#define TINO_GETOPT_UCHAR	"C\1"	/* unsigned character	*/
#define TINO_GETOPT_CHAR	"c\1"	/* character	*/

/* Numeric types (default to 0).
 * The unsigned versions do not accept any signs.
 * The signed version do accept + and -.
 */
#define TINO_GETOPT_UBYTE	"B\1"	/* byte 0..255	*/
#define TINO_GETOPT_BYTE	"b\1"	/* signed byte -128..127	*/
#define TINO_GETOPT_USHORT	"W\1"	/* unsigned short int / word	*/
#define TINO_GETOPT_SHORT	"w\1"	/* short int / signed word	*/
#define TINO_GETOPT_UNSIGNED	"u\1"	/* unsigned int	*/
#define TINO_GETOPT_INT		"i\1"	/* int	*/
#define TINO_GETOPT_ULONGINT	"U\1"	/* unsigned long	*/
#define TINO_GETOPT_LONGINT	"I\1"	/* long	*/
#define TINO_GETOPT_ULLONG	"L\1"	/* unsigned long long	*/
#define TINO_GETOPT_LLONG	"l\1"	/* long long	*/

/* A "help" option.
 *
 * When this option is present, the given string is the usage.
 * Default value for options, so active with TINO_GETOPT_USAGE.
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
      if (p->fDEBUG)							\
        fprintf(stderr, "getopt: " #X "\n");				\
      Y;								\
      arg+=(sizeof TINO_GETOPT_##X)-1;					\
    }									\
  else

#define IFarg(X)   do { if (((p->f##X)=va_arg(*list,tino_getopt_##X *))==0) return 0; } while(0)
#define IFflg(X)   IFgen(X,(p->f##X)=1)
#define IFptr(X)   IFgen(X,IFarg(X))
#define IFtyp(X)   IFgen(X,p->var.type=TINO_GETOPT_TYPE_##X)

enum tino_getopt_type
  {
    TINO_GETOPT_TYPE_HELP,
    TINO_GETOPT_TYPE_FLAG,
    TINO_GETOPT_TYPE_STRING,
    TINO_GETOPT_TYPE_UCHAR,
    TINO_GETOPT_TYPE_CHAR,
    TINO_GETOPT_TYPE_UBYTE,
    TINO_GETOPT_TYPE_BYTE,
    TINO_GETOPT_TYPE_USHORT,
    TINO_GETOPT_TYPE_SHORT,
    TINO_GETOPT_TYPE_UNSIGNED,
    TINO_GETOPT_TYPE_INT,
    TINO_GETOPT_TYPE_ULONGINT,
    TINO_GETOPT_TYPE_LONGINT,
    TINO_GETOPT_TYPE_ULLONG,
    TINO_GETOPT_TYPE_LLONG,
  };

union tino_getopt_types
  {
    void *			S;
    const char *		s;
    unsigned char		C;
    char			c;
    unsigned short		W;
    short			w;
    unsigned			u;
    int				i;
    unsigned long		U;
    long			I;
    unsigned long long		L;
    long long			l;
  };

struct tino_getopt_val
  {
    enum tino_getopt_type	type;
    union tino_getopt_types	*ptr;
#if 0
    union tino_getopt_types	val;
#endif
  };

#define fGENERIC_PTR	var.ptr

typedef union tino_getopt_types	tino_getopt_GENERIC_PTR;
typedef const char		tino_getopt_VALID_STR;
typedef	void			tino_getopt_USER;
typedef const char *		tino_getopt_FN(void *, char **, const char *, void *);
typedef int			tino_getopt_CB(int, char **, int, void *);

struct tino_getopt_impl
  {
    const char	*arg;		/* the argument string	*/
    struct tino_getopt_val	var;	/* the pointer to the argument variable	*/
    struct tino_getopt_val	min, max;	/* the min/max values	*/

    /*
     * broken up
     */
    const char	*unknown;	/* pointer to the first unknown	*/
    const char	*opt;		/* pointer to the option name	*/
    int		optlen;		/* length of the option name	*/
    const char	*help;		/* pointer to help string	*/
    /* flags
     */
    int		fDEBUG, fNODEFAULT, fDEFAULT, fUSAGE;
    int		fTAR, fPOSIX, fPLUS, fLOPT, fLLOPT, fDIRECT, fDD;
    const char	*fVALID_STR;
    /* pointers
     */
    tino_getopt_USER	*fUSER;
    tino_getopt_FN	*fFN;
    tino_getopt_CB	*fCB;
  };


/**********************************************************************/
/**********************************************************************/

/* This parses the current option and fills tino_getopt_impl It
 * fetches all the options, arguments and variable pointers from the
 * va_list, except for the default value in case TINO_GETOPT_DEFAULT
 * is used.
 */
static struct tino_getopt_impl *
tino_getopt_arg(struct tino_getopt_impl *p, va_list *list, const char *arg)
{
  if (!arg)
    arg	= va_arg(*list, const char *);
  p->unknown	= 0;
  p->help	= 0;
  p->opt	= 0;
  p->var.type	= (enum tino_getopt_type)0;
  p->min.type	= (enum tino_getopt_type)0;
  p->max.type	= (enum tino_getopt_type)0;
  p->arg	= arg;
  if (!arg)
    return 0;

  /* Parse all the possible flags
   */
  for (;;)
    IFflg(DEBUG)
      IFflg(TAR)
      IFflg(POSIX)
      IFflg(PLUS)
      IFflg(LOPT)
      IFflg(LLOPT)
      IFflg(DIRECT)
      IFflg(DD)
      IFflg(USAGE)
      IFflg(DEFAULT)
      IFflg(NODEFAULT)
      IFtyp(HELP)
      IFptr(USER)
      IFptr(FN)
      IFptr(CB)
      IFptr(VALID_STR)
      IFtyp(FLAG)
      IFtyp(STRING)
      IFtyp(UCHAR)
      IFtyp(CHAR)
      IFtyp(UBYTE)
      IFtyp(BYTE)
      IFtyp(USHORT)
      IFtyp(SHORT)
      IFtyp(UNSIGNED)
      IFtyp(INT)
      IFtyp(ULONGINT)
      IFtyp(LONGINT)
      IFtyp(ULLONG)
      IFtyp(LLONG)
    {
#if 0
    IF(MIN)
      IF(MAX)
      IF(MIN_PTR)
      IF(MIN_PTR)
#endif
      /* not found	*/
      p->opt	= arg;
      for (;;)
	{
	  switch (*arg++)
	    {
	      /* Skip the option text.  It must not contain a \1
	       */
	    default:
	      continue;

	      /* After a TAB (option without argument) or a space
	       * (argument) the help text follows.
	       */
	    case '\t':
	    case ' ':
	      p->help	= arg-1;
	      if (p->fDEBUG)
		fprintf(stderr, "getopt help: '%s'\n", arg);
	      /* If we hit the end of the string there is no help text
	       * (be graceful and accept it)
	       */
	    case 0:
	      p->optlen	= arg-p->opt-1;
	      if (p->fDEBUG)
		fprintf(stderr, "getopt option: '%.*s'\n", p->optlen, p->opt);
	      /* Fetch the pointer to the variable
	       * As we do not know which variable this is, do it a generic way
	       */
	      if (p->var.type)
		IFarg(GENERIC_PTR);
	      return p;

	      /* We hit a \1 which means, we hit something we do not understand.
	       * p->opt points to the start of this crap.
	       * have grace: skip everything which is unknown
	       */
	    case '\1':
	      if (p->fDEBUG)
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

	case '\n':
	case '\t':
	  *set	= tmp;
	  return tmp-ptr-1;
	}
    }
}

#define TINO_GETOPT_AUXBUF_SIZE	40

/* assembles the value in auxbuf,
 * except for the string case.
 */
static const char *
tino_getopt_var_to_str(struct tino_getopt_impl *p, char auxbuf[TINO_GETOPT_AUXBUF_SIZE])
{
  switch (p->var.type)		/* SORRY!!!	*/
    {
      long long			lld;
      unsigned long long	llu;
      int			i;

    case TINO_GETOPT_TYPE_STRING:	return p->var.ptr->s;

    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      i	= p->var.ptr->C+1;
      if (i==128)
	i	= 0;
      if (i<=32)
	{
	  strncpy(auxbuf, "-0------abtnvfr", TINO_GETOPT_AUXBUF_SIZE);
	  snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE,
		   "%.3s 0x%02x %d%c'\\%c'",
		   "DELNULSOHSTXETXEOTENQACKBELBS\0HT\0LF\0VT\0FF\0CR\0SO\0SI\0"
		   "DLEDC1DC2DC3DC4NAKSYNETBCANEM\0SUBESCFS\0GS\0RS\0US\0"
		   +i*3,
		   p->var.ptr->C,
		   p->var.ptr->C,
		   (auxbuf[i] && auxbuf[i]!='-' ? ' ' : 0),
		   auxbuf[i]);
	  break;
	}
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE,
	       "'%c' 0x%02x %d",
	       p->var.ptr->c,
	       p->var.ptr->C,
	       p->var.ptr->C);
      break;

    case TINO_GETOPT_TYPE_HELP:		strcpy(auxbuf, "(has no default)");	break;
    case TINO_GETOPT_TYPE_FLAG:		strcpy(auxbuf, p->var.ptr->u ? "SET(1)" : "UNSET(0)"); if (p->var.ptr->u<=1) break;
      /* fallthrough!	*/
    case TINO_GETOPT_TYPE_INT:		lld	= p->var.ptr->i;	if (0)
    case TINO_GETOPT_TYPE_BYTE:		lld	= p->var.ptr->c;	if (0)
    case TINO_GETOPT_TYPE_SHORT:	lld	= p->var.ptr->w;	if (0)
    case TINO_GETOPT_TYPE_LONGINT:	lld	= p->var.ptr->I;	if (0)
    case TINO_GETOPT_TYPE_LLONG:	lld	= p->var.ptr->l;
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE, "%lld", lld);
      break;

    case TINO_GETOPT_TYPE_UNSIGNED:	llu	= p->var.ptr->u;	if (0)
    case TINO_GETOPT_TYPE_UBYTE:	llu	= p->var.ptr->C;	if (0)
    case TINO_GETOPT_TYPE_USHORT:	llu	= p->var.ptr->W;	if (0)
    case TINO_GETOPT_TYPE_ULONGINT:	llu	= p->var.ptr->U;	if (0)
    case TINO_GETOPT_TYPE_ULLONG:	llu	= p->var.ptr->L;
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE, "%llu", llu);
      break;
    }
  return auxbuf;
}

static void
tino_getopt_var_set_varg(struct tino_getopt_impl *p, va_list *list)
{
  if (p->fDEBUG)
    fprintf(stderr, "getopt: preset opt %.*s to given default value\n", p->optlen, p->opt);
  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      p->var.ptr->i	= va_arg(*list, int);
      break;

    case TINO_GETOPT_TYPE_STRING:
      p->var.ptr->s	= va_arg(*list, char *);
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      p->var.ptr->c	= va_arg(*list, int);
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      p->var.ptr->w	= va_arg(*list, int);
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      p->var.ptr->I	= va_arg(*list, long);
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      p->var.ptr->l	= va_arg(*list, long long);
      break;
	  
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
}

static void
tino_getopt_var_set_0(struct tino_getopt_impl *p)
{
  if (p->fDEBUG)
    fprintf(stderr, "getopt: nulling opt %.*s\n", p->optlen, p->opt);
  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      p->var.ptr->i	= 0;
      break;

    case TINO_GETOPT_TYPE_STRING:
      p->var.ptr->s	= 0;
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      p->var.ptr->c	= 0;
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      p->var.ptr->w	= 0;
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      p->var.ptr->I	= 0;
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      p->var.ptr->l	= 0;
      break;
	  
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
}

/* This is not ready yet
 */
static int
tino_getopt_var_set_arg(struct tino_getopt_impl *p, const char *arg, const char *next)
{
  unsigned long long	ull;
  int			n;

  n	= 1;
  p->fDEFAULT	= 0;
  p->fNODEFAULT	= 0;
  if ((p->fLLOPT || p->fLOPT || p->fDD) && *arg)
    {
      /* Long options have --"long"=arg or --"long."arg
       */
      if (isalnum(p->opt[p->optlen-1]))
	arg++;
    }
  else if (!*arg && !p->fDIRECT)
    {
      n		= 2;
      arg	= next;
      if (!arg)
	arg	= "";
    }
  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_HELP:
      if (p->fDEBUG)
	fprintf(stderr, "getopt: request usage via %.*s\n", p->optlen, p->opt);
      return -1;

    case TINO_GETOPT_TYPE_FLAG:
      if (p->fDEBUG)
	fprintf(stderr, "getopt: setting flag %.*s\n", p->optlen, p->opt);
      p->var.ptr->i	= 1;
      return 0;

    case TINO_GETOPT_TYPE_STRING:
      if (p->fDEBUG)
	fprintf(stderr, "getopt: set opt %.*s to string '%s'\n", p->optlen, p->opt, arg);
      p->var.ptr->s	= arg;
      return n;

    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      if (p->fDEBUG)
	fprintf(stderr, "getopt: set opt %.*s to char '%c'\n", p->optlen, p->opt, *arg);
      p->var.ptr->c	= *arg;
      return n;

    default:
      break;
    }

  if (p->fDEBUG)
    fprintf(stderr, "getopt: setting opt %.*s (%s)\n", p->optlen, p->opt, arg);

  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_HELP:
    case TINO_GETOPT_TYPE_FLAG:
    case TINO_GETOPT_TYPE_STRING:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      return -1;

    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_ULLONG:
      ull	= strtoull(arg, NULL, 0);
      break;

    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_SHORT:
    case TINO_GETOPT_TYPE_LONGINT:
    case TINO_GETOPT_TYPE_LLONG:
      ull	= strtoll(arg, NULL, 0);
      break;
    }

  /* check if the type fits into the argument
   * warn on overflow
   *
   * MISSING
   */
  TINO_XXX;	/* do some type and overflow checking	*/

  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      p->var.ptr->i	= ull;
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      p->var.ptr->c	= ull;
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      p->var.ptr->w	= ull;
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      p->var.ptr->I	= ull;
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      p->var.ptr->l	= ull;
      break;
	  
    case TINO_GETOPT_TYPE_STRING:
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
  return n;
}

#if 0
static char
tino_getopt_printchar(char c)
{
  return isprint(c) ? c : '.';
}

static int
tino_getopt_var_print(FILE *fd, const char *prefix, const struct tino_getopt_impl *p)
{
  switch (p->var.type)
    {
    case TINO_GETOPT_TYPE_HELP:
      return -1;

    case TINO_GETOPT_TYPE_UNSIGNED:
      fprintf(fd, "%s%u", prefix, p->var.ptr->u);
      break;

    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      fprintf(fd, "%s%d", prefix, p->var.ptr->i);
      break;

    case TINO_GETOPT_TYPE_STRING:
      fprintf(fd, "%s%s", prefix, p->var.ptr->s);
      break;

    case TINO_GETOPT_TYPE_UBYTE:
      fprintf(fd, "%s%u(%c)", prefix, p->var.ptr->C, tino_getopt_printchar(p->var.ptr->c));
      break;

    case TINO_GETOPT_TYPE_BYTE:
      fprintf(fd, "%s%d(%c)", prefix, p->var.ptr->c, tino_getopt_printchar(p->var.ptr->c));
      break;
      
    case TINO_GETOPT_TYPE_UCHAR:
      fprintf(fd, "%s%c(%u)", prefix, tino_getopt_printchar(p->var.ptr->c), p->var.ptr->C);
      break;

    case TINO_GETOPT_TYPE_CHAR:
      fprintf(fd, "%s%c(%d)", prefix, tino_getopt_printchar(p->var.ptr->c), p->var.ptr->c);
      break;

    case TINO_GETOPT_TYPE_USHORT:
      fprintf(fd, "%s%u", prefix, p->var.ptr->W);
      break;

    case TINO_GETOPT_TYPE_SHORT:
      fprintf(fd, "%s%d", prefix, p->var.ptr->w);
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
      fprintf(fd, "%s%lu", prefix, p->var.ptr->U);
      break;

    case TINO_GETOPT_TYPE_LONGINT:
      fprintf(fd, "%s%ld", prefix, p->var.ptr->I);
      break;

    case TINO_GETOPT_TYPE_ULLONG:
      fprintf(fd, "%s%llu", prefix, p->var.ptr->L);
      break;
	  
    case TINO_GETOPT_TYPE_LLONG:
      fprintf(fd, "%s%lld", prefix, p->var.ptr->l);
      break;
    }
  return 0;
}
#endif

#ifndef TINO_MAXOPTS
#define	TINO_MAXOPTS	256	/* Yeah, I need to get rid of this sometimes	*/
#endif

/* returns:
 * - Offset in the arguments where the non-options start.
 * - or 0 on usage or other things which are no real error.
 * - or -1 on error.
 *
 * This routine is too long.
 */
static int
tino_getopt(int argc, char **argv,	/* argc,argv as in main	*/
	    int min, int max,		/* min..max args, max<min: unlimited */
	    const char *global		/* string of global settings	*/
	    /* append the general commandline usage to global (with a SPC) */
	    , ...
	    /* Now following "pairs" follow:
	     * A flag description string.
	     * optional FN or USER pointers as in description string.
	     * A pointer to the flag,
	     * optionally the DEFAULT value.
	     */
	    /* TERMINATE THIS WITH A NULL !!! */
	    )
{
  struct tino_getopt_impl	q[TINO_MAXOPTS];
  va_list			list;
  int				verslen, complen;
  int				opts, i, pos;
  int				tarlike, posixlike, pluslike, ddlike, llike, lllike;
  const char			*compiled, *rest, *arg0, *usage;

  /* Parse the first argument (global):
   * Version
   * Compilation date
   * Global options
   * Usage string
   */
  va_start(list, global);
  verslen	= tino_getopt_tab(global, &compiled);
  complen	= tino_getopt_tab(compiled, &rest);
  memset(q,0,sizeof *q);
  tino_getopt_arg(q, &list, rest);

  /* Parse all the possible arguments into an array
   */
  tarlike	= 0;
  posixlike	= 0;
  pluslike	= 0;
  ddlike	= 0;
  llike		= 0;
  lllike	= 0;
  for (opts=0; ++opts<sizeof q/sizeof *q; )
    {
      /* copy global settings
       */
      q[opts]	= q[0];
      if (!tino_getopt_arg(q+opts, &list, NULL))
	break;
      /* Preset the variables
       */
      if (!q[opts].fNODEFAULT)
	{
	  if (q[opts].fDEFAULT)
	    tino_getopt_var_set_varg(q+opts, &list);
	  else
	    tino_getopt_var_set_0(q+opts);
	}
      /* for below
       */
      tarlike	|= q[opts].fTAR;
      posixlike	|= q[opts].fPOSIX;
      pluslike	|= q[opts].fPLUS;
      ddlike	|= q[opts].fDD;
      llike	|= q[opts].fLOPT;
      lllike	|= q[opts].fLLOPT;
    }
  va_end(list);

  if (opts==sizeof q/sizeof *q)
    fprintf(stderr, "getopt: too many builtin options, increase MAXOPTS, continuing anyway\n");

  if (tarlike || posixlike || pluslike)
    fprintf(stderr, "getopt: tar/posix/plus not yet implemented, continuing anyway\n");

  if (tarlike && argc>1 && argv[1][0]!='-')
    {
      /* First option is TAR options
       * Hunt through all the TAR options and process them ..
       * (or do this below?)
       */
      TINO_XXX;
    }

  /* Well, what follows is too long actually.
   * However it's difficult to break it up.
   */
  for (pos=0; pos>=0 && ++pos<argc; )
    {
      const char	*ptr;

      ptr	= argv[pos];
      /* - for it's own always is an ARG
       */
      if (*ptr=='-' && *++ptr)
	{
	  if (*ptr=='-')
	    {
	      /* end of options: --
	       * Make it unknown in situations where we do not process '-' at all,
	       * so it becomes an ARG
	       */
	      if (!*++ptr && (posixlike || pluslike || llike || lllike || !(ddlike || tarlike)))
		{
		  pos++;
		  break;
		}
	      /* long option
	       */
	      for (i=opts;;)
		{
		  if (--i<1)
		    {
		      fprintf(stderr, "getopt: unknown option --%s\n", ptr);
		      pos	= -1;
		      break;
		    }

#define	TINO_CMPLONGOPT(I)	(!q[I].optlen || strncmp(ptr, q[I].opt, q[I].optlen) ||	\
	  (ptr[q[I].optlen] && ptr[q[I].optlen]!='=' && isalnum(q[I].opt[q[I].optlen-1])))

#define	TINO_PROCESSLONGOPT(I,COND)							\
	      if (!(COND) || TINO_CMPLONGOPT(I))					\
		continue;								\
	      ptr	+= q[I].optlen;							\
	      I		= tino_getopt_var_set_arg(q+I, ptr, argv[pos+1]);		\
	      if (!I && *ptr)								\
		{									\
		  fprintf(stderr, "getopt: flag %s must not have args\n", argv[pos]);	\
		  pos	= -1;								\
                  break;								\
		}
              /* i<0	help option or error
	       * i==0	last thing was flag
	       * i==1	last thing was argument
	       * i==2	one addional argv was eaten away
	       */
	  
		  TINO_PROCESSLONGOPT(i,q[i].fLLOPT);
		  break;
		}
	      /* Either pos<0 (error)
	       * or the option has been processed.
	       */
	    }
	  else
	    {
	      /* short option (preceeding is -)
	       * Note that *ptr must be != 0 here.
	       */
	      do
		{
		  i=opts;
		  do
		    {
		      if (--i<1)
			{
			  fprintf(stderr, "getopt: unknown option -%s\n", ptr);
			  pos	= -1;
			  break;
			}
		    } while (((q[i].fLLOPT || q[i].fDD) && !q[i].fLOPT) ||
			     !q[i].optlen || strncmp(ptr, q[i].opt, q[i].optlen));
		  if (pos<0)
		    break;
		  ptr	+= q[i].optlen;
		  i	= tino_getopt_var_set_arg(q+i, ptr, argv[pos+1]);
		  if (i)
		    break;
		} while (*ptr);
	      /* Either pos<0 (eroor)
	       * or all optons have been processed.
	       */
	    }
	  /* short or long option
	   * i<0	help option or error
	   * i==0	last thing was flag
	   * i==1	last thing was argument
	   * i==2	one addional argv was eaten away
	   */
	  if (i<0 || pos<0)
	    {
	      pos	= -1;
	      break;
	    }
	  if (i>1)
	    pos++;
	  continue;
	}

      /* hunt for DD like options
       */
      if (ddlike)
	{
	  for (i=opts; --i>0; )
	    {
	      TINO_PROCESSLONGOPT(i,q[i].fDD);
              /* i<0	help option or error
	       * i==0	last thing was flag
	       * i==1	last thing was argument
	       * i==2	one addional argv was eaten away
	       */
	      if (i<0)
		{
		  pos	= -1;
		  break;
		}
	      if (i>1)
		pos++;
	      i	= 1;
		
	      break;
	    }
	  if (pos<0)
	    break;
	  if (i>0)
	    continue;

	  /* Fallthrough in case nothing found
	   */
	}

      /* non-option argument
       */
      TINO_XXX;	/* check for TAR like options here	*/

      /* Call the argument callback if defined.
       */
      if (q[0].fCB && (i=(q[0].fCB)(pos, argv, argc, q[0].fUSER))!=0)
	{
	  if (i<0)
	    pos	= 0;
	  pos	+= i;
	  continue;
	}
      if (!posixlike)
	break;

      /* Not yet implemented
       * reorder the options (POSIX) ..
       */
      TINO_XXX;
      break;
    }

  /* If number of arguments are ok, just return the offset.
   */
  if (pos>0 && argc-pos>=min && (max<min || argc-pos<=max))
    return pos;

  /* Print usage
   */
  if (!q[0].help)
    q[0].help	= "";
  usage	= q[0].help;
#ifndef TINO_GETOPT_USAGE_FULL_PATH
  if ((arg0=strrchr(argv[0], '/'))!=0 ||	/* Unix	*/
      (arg0=strrchr(argv[0], '\\'))!=0)		/* Windows, sigh	*/
    arg0++;
  else
#endif
    arg0	= argv[0];

  fprintf(stderr,
	  "Usage: %s [options]%.*s\n"
	  "\t\tversion %.*s compiled %.*s"
	  , arg0, tino_getopt_tab(usage, &usage), q[0].help,
	  verslen, global,
	  complen, compiled);
  if (*usage)
    {
      usage--;	/* bing back the LF	*/
      do
	{
	  char	buf[130], *tmp;

	  strncpy(buf, usage, sizeof buf);
	  buf[(sizeof buf)-1]	= 0;
	  tmp	= strchr(buf, '\n');
	  if (tmp)
	    *++tmp	= 0;
	  usage	+= strlen(buf);
	  fprintf(stderr, buf, arg0);
	} while (*usage);
    }
  fprintf(stderr, "\nOptions:\n");
  for (i=1; i<opts; i++)
    {
      char	auxbuf[TINO_GETOPT_AUXBUF_SIZE];

      fputc('\t', stderr);
      if (!q[i].fDD)
	{
	  if (q[i].fLLOPT)
	    fputc('-', stderr);
	  fputc('-', stderr);
	}
      fprintf(stderr, "%s\n", q[i].opt);
      if (q[i].fNODEFAULT || q[i].fDEFAULT)
	{
	  const char	*s;

	  fprintf(stderr, "\t\t(default ");
	  s	= tino_getopt_var_to_str(q+i, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s)\n" : "'%s')\n"), s);
	}
    }
  return 0;	/* usage printed	*/
}

#ifdef TINO_TEST_MAIN
/* Take this as an Example.
 *
 * If you consider this complex, please show me how this can be
 * implemented even more efficient.  But it must be as easy as
 * scanf(), or more easy, so using some array disqualifies instantly.
 * And it must be efficient, too, so nothing like:
 *
 * .., "min", 0, "max", 100, "i", &i, ..
 *
 * as this needs 6 stack arguments, instead of my variant:
 *
 * .., "min\1max\1i\1n nr\tnumber from 0 to 100", 0, 100, &i, ..
 *
 * which only needs 4 stack arguments.  I already think of extending
 * this to
 *
 * .., "min0\1max100\1n nr\tnumber from 0 to 100", &i, ..
 *
 * where the numbers replace the stack arguments, but this has not yet
 * been implemented.  I will do, as I really like it that you can
 * "hack" argument processing using a hex editor this way.
 */
int
main(int argc, char **argv)
{
  int		argn, flag, i;
  const char	*str;
  char		auxbuf[TINO_GETOPT_AUXBUF_SIZE];
  union tino_getopt_types	c;
  struct tino_getopt_impl	aux;

  str	= "(this value was not inititialized)";
  argn	= tino_getopt(argc, argv, 1, 2,
		      TINO_GETOPT_VERSION("unit.test")
		      TINO_GETOPT_DEBUG
		      " one [two]",

		      TINO_GETOPT_USAGE
		      "h	this help"
		      ,

		      TINO_GETOPT_CHAR
		      "c char	a char"
		      , &c.c,

		      TINO_GETOPT_STRING
		      TINO_GETOPT_NODEFAULT
		      "s str	fetch a string"
		      , &str,

		      TINO_GETOPT_FLAG
		      "f	set a flag"
		      , &flag,

		      TINO_GETOPT_INT
		      TINO_GETOPT_DEFAULT
#if 0
		      TINO_GETOPT_INT_MIN
		      TINO_GETOPT_INT_MAX
#endif
		      "n nr	number from 0 to 100"
#if 0
		      , 0
		      , 100
#endif
		      , &i,
		      20,

		      NULL
		      );
  if (argn<=0)
    return 1;
  
  aux.var.ptr	= &c;
  aux.var.type	= TINO_GETOPT_TYPE_CHAR;

  printf("argc:   %d\n", argc);
  printf("argn:   %d\n", argn);
  printf("string: %s\n", str);
  printf("flag:   %d\n", flag);
  printf("int:    %d\n", i);
  printf("char:   %s\n", tino_getopt_var_to_str(&aux, auxbuf));
  for (; argn<argc; argn++)
    printf("arg%03d: %s\n", argn, argv[argn]);
  return 0;
}
#endif

#endif
