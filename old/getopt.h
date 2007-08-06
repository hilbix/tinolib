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
 * Copyright (C)2004-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.33  2007-08-06 09:28:22  tino
 * Improvements in debug output
 *
 * Revision 1.32  2007/06/01 09:35:36  tino
 * New features in getopt introduced
 *
 * Revision 1.31  2007/04/08 10:27:02  tino
 * Too many args added
 *
 * Revision 1.30  2007/04/04 05:28:25  tino
 * See ChangeLog
 *
 * Revision 1.29  2007/04/03 02:19:19  tino
 * bugfix, forgot no suffix case
 *
 * Revision 1.28  2007/04/03 00:40:34  tino
 * See ChangeLog
 *
 * Revision 1.27  2007/03/03 16:16:42  tino
 * Improved getopt: MIN/MAX and Help only printed if help option present
 *
 * Revision 1.26  2007/01/25 04:40:49  tino
 * Improvements in getopt and standard "main" routines (error-behavior).
 * getopt not yet completely ready, commit because this here works again (mostly).
 *
 * Revision 1.25  2007/01/22 19:04:22  tino
 * User-arg added to hook
 *
 * Revision 1.24  2006/10/04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 * Revision 1.23  2006/10/03 21:44:10  tino
 * Compile warnings for Ubuntu removed
 *
 * Revision 1.22  2006/10/03 21:00:05  tino
 * TINO_GETOPT_FN implemented
 *
 * Revision 1.21  2006/08/24 22:21:33  tino
 * More things commented away as they are not needed now
 *
 * Revision 1.20  2006/08/24 01:00:17  tino
 * Internally restructured and hook added
 *
 * Revision 1.19  2006/07/25 20:53:04  tino
 * see ChangeLog
 *
 * Revision 1.18  2006/07/22 23:47:44  tino
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

#include "arg.h"

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
 * you have to check for fUSAGE yourself.
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

/** Set parameter default value.
 *
 * With NODEFAULT, the variables will not be initialized (takes
 * precedence).  With DEFAULT a default value will be fetched AFTER
 * the variable and all other options (so it's always last!).  The
 * default depends on the size of the variable, so beware of long long
 * (use 0ll) and pointers (use NULL, not 0)!
 *
 * IMPORTANT: DEFAULT must be given as the last options, which come
 * after the variable pointer, else you will get segmentation
 * violation!  This is because the type of the variable to fetch is
 * only known after everything else has been parsed.
 *
 * Implementation note:
 *
 * You can give TINO_GETOPT_DEFAULT globally, such that all parameters
 * must have default values.  You can suppress the need for those
 * default parameters using TINO_GETOPT_NODEFAULT, however
 * TINO_GETOPT_NODEFAULT suppresses that the variable is NULLed.
 *
 * Note that TINO_GETOPT_DEFAULT_PTR takes precedence over
 * TINO_GETOPT_NODEFAULT. TINO_GETOPT_DEFAULT_ENV and
 * TINO_GETOPT_DEFAULT_PTR are mutually exclusive.
 */
#define	TINO_GETOPT_NODEFAULT	"keep\1"/* do not init variable	*/
#define	TINO_GETOPT_DEFAULT	"def\1"	/* give variable defaults */
#define	TINO_GETOPT_DEFAULT_PTR	"DEF\1"	/* pointer to default */
/* not yet implemented: */
#define TINO_GETOPT_DEFAULT_ENV	"env\1"	/* take default from env-var */

/* Min and Max parameters.
 * Must be followed by a Numeric Type from above.
 *
 * The PTR version uses a pointer arg which points to the data.  This
 * pointer is read immediately when this option is requested!
 *
 * IMPORTANT: MIN, MAX must be given AFTER the variable pointer, in
 * the sequence DEFAULT, MIN, MAX, else you will get segmentation
 * violation!  THIS IS NOT TRUE FOR THE *_PTR CASE!
 *
 * If MIN and MIN_PTR are given (only one is possible) then the
 * tighter values take precedence.
 *
 * MIN AND MAX ARE CURRENTLY ONLY IMPLEMENTED FOR TINO_GETOPT_FLAG!
 */
#define	TINO_GETOPT_MIN		"min"
#define	TINO_GETOPT_MAX		"max"
#define	TINO_GETOPT_MIN_PTR	"MIN"
#define	TINO_GETOPT_MAX_PTR	"MAX"


/**********************************************************************/
/* Arguments
 */

/** Integer flag
 *
 * Needs a pointer to:
 *	int
 * If option present:
 *	The integer will be set to 1 (or see below)
 * else:
 *	The integer will be set to 0 (or the default)
 *
 * If MIN is present, the flag will jump from the preset (0 or
 * default) to this MIN value (this is, it is outside MIN..MAX).
 *
 * If MAX is present, it will increment to MAX on each flag occurence
 * (or if MAX is less than MIN or the default it will decrement).
 *
 * If you only set MIN, this is the "other" value to which the Flag is
 * set on the option.  If you only set MAX the flag value will count
 * the number of flags up to the maximum.  If you set both you will
 * have the "jump" behavior.
 */
#define TINO_GETOPT_FLAG	"f\1"

/** Stringflags
 *
 * Needs a pointer to:
 *	const char *
 * If option present:
 *	Pointer is set to the option string, which is \1 terminated!
 * If you want something else:
 *	Give value as TINO_GETOPT_MIN, see below for TINO_GETOPT_MAX
 * If you want to set a default somewhere instead of NULL:
 *	Use TINO_GETOPT_STRINGFLAGS for all 
 *
 * If you want to give a default to the flag, there is a
 * convenience TINO_GETOPT_STRINGFLAGS which includes the NODEFAULT.
 * You then use STRINGFLAGS for all flags except the one with the
 * TINO_GETOPT_DEFAULT setting.
 *
 * TINO_GETOPT_MIN gives the flag value.
 *
 * FUTURE SUPPORT (TINO_GETOPT_MAX NOT YET IMPLEMENTED):
 *
 * If TINO_GETOPT_MAX is given (currently ignored), it gives the
 * endpointer of the string list which are given in TINO_GETOPT_MIN.
 * Such a list is organized like the environment: "value1\0value2\0".
 * TINO_GETOPT_MAX then points to the end of the list (this is behind
 * the value\0 in the example).  The flag gets then assigned all the
 * value in sequence until the MAX value is reached, which is kept (it
 * then sticks to the "value2" in the example).
 *
 * If TINO_GETOPT_MIN and TINO_GETOPT_MAX can reverse roles, that is,
 * if TINO_GETOPT_MIN is higher than TINO_GETOPT_MAX then the flag
 * gets assigned the last value first with the sequence going to the
 * start of the string.
 *
 * If TINO_GETOPT_MIN is set to NULL or TINO_GETOPT_MAX is NULL, then
 * the end of the string of the other variable is located for the
 * first double NUL ('\0\0') sequence (like in the environment
 * string).  So if you give "value1\0\value2\0" as TINO_GETOPT_MIN
 * (note the \0 before the " to terminate the string with a
 * double-NUL) you can give NULL for TINO_GETOPT_MAX for convenience.
 *
 * Again: This is not yet implemented!  This is only to explain, why
 * the "default" is given as _MIN which might seem unlogically else.
 */
#define TINO_GETOPT_STRINGFLAG	"F\1"
#define TINO_GETOPT_STRINGFLAGS	"F\1" TINO_GETOPT_NODEFAULT

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

#if 0
/* Give valid data for strings:
 * This reads away a pointer to NUL terminated strings.
 * The list must be terminated by NUL NUL.
 * Thus to include the empty string "", do it as the first string:
 * TINO_GETOPT_VALID_STR TINO_GETOPT_STR, "\0val1\0val2\0", &str,
 */
#define	TINO_GETOPT_VALID_STR	"val\1"
#endif

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

/* Allow bkmgt suffixes to numbers
 */
#define TINO_GETOPT_SUFFIX	"bkmgt\1"

/* Allow smhdw suffixes to numbers as time offsets
 */
#define TINO_GETOPT_TIMESPEC	"smhdw\1"

/* Ignore errors while parsing this option
 */
#define TINO_GETOPT_IGNERR	"ign\1"


/**********************************************************************/
/**********************************************************************/

/* Process the arg.
 * Returns NULL if end of prefixes.
 */
#define	TINO_GETOPT_IFgen(X,Y)						\
  if (!strncmp(arg, TINO_GETOPT_##X, (sizeof TINO_GETOPT_##X)-1))	\
    {									\
      if (p->DEBUG_var)							\
        fprintf(stderr, "getopt debug: " #X "\n");			\
      Y;								\
      arg+=(sizeof TINO_GETOPT_##X)-1;					\
    }									\
  else

#define TINO_GETOPT_IFarg(X)	do { if (((p->X##_var)=TINO_VA_ARG(list,tino_getopt_##X *))==0) return 0; } while(0)
#define TINO_GETOPT_IFflg(X)	TINO_GETOPT_IFgen(X,(p->X##_var)=1)
#define TINO_GETOPT_IFptr(X)	TINO_GETOPT_IFgen(X,TINO_GETOPT_IFarg(X))
#define TINO_GETOPT_IFtyp(X)	TINO_GETOPT_IFgen(X,p->type=TINO_GETOPT_TYPE_##X)

enum tino_getopt_type
  {
    TINO_GETOPT_TYPE_HELP,
    TINO_GETOPT_TYPE_FLAG,
    TINO_GETOPT_TYPE_STRINGFLAG,
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

#define TINO_GETOPT_GENERIC_PTR_var	varptr
typedef union tino_getopt_types	tino_getopt_TINO_GETOPT_GENERIC_PTR, tino_getopt_MIN_PTR, tino_getopt_MAX_PTR, tino_getopt_DEFAULT_PTR;
#if 0
typedef const char		tino_getopt_VALID_STR;
#endif
typedef	void			tino_getopt_USER;
typedef const char *		tino_getopt_FN(void *, const char *, const char *, void *);
typedef int			tino_getopt_CB(int, char **, int, void *);

struct tino_getopt_impl
  {
#if 0
    const char	*arg;		/* the argument string	*/
#endif
    enum tino_getopt_type	type;		/* type of argument var	*/
    union tino_getopt_types	*varptr;	/* pointer to argument var	*/
    union tino_getopt_types	min, max;	/* the min/max values	*/
    tino_getopt_MIN_PTR		*MIN_PTR_var;	/* as pointers	*/
    tino_getopt_MAX_PTR		*MAX_PTR_var;
    tino_getopt_DEFAULT_PTR	*DEFAULT_PTR_var;

    /*
     * broken up
     */
#if 0
    const char	*unknown;	/* pointer to the first unknown	*/
#endif
    const char	*opt;		/* pointer to the option name	*/
    int		optlen;		/* length of the option name	*/
#if 0
    const char	*help;		/* pointer to help string	*/
#endif
    /* flags
     */
    int		DEBUG_var, NODEFAULT_var, DEFAULT_var, USAGE_var;
    int		TAR_var, POSIX_var, PLUS_var, LOPT_var, LLOPT_var, DIRECT_var, DD_var;
    int		MIN_var, MAX_var;
    int		SUFFIX_var, TIMESPEC_var, IGNERR_var;
#if 0
    const char	*VALID_STR_var;
#endif
    /* pointers
     */
    tino_getopt_USER	*USER_var;
    tino_getopt_FN	*FN_var;
    tino_getopt_CB	*CB_var;
  };


/**********************************************************************/
/**********************************************************************/

/* Print the correct option name (including - or -- prefix)
 *
 * After the option name the SUFFIX is printed.
 */
static void
tino_getopt_print_option_name(struct tino_getopt_impl *q, int full, const char *suffix)
{
  if (!q->DD_var)
    {
      if (q->LLOPT_var)
	fputc('-', stderr);
      fputc('-', stderr);
    }
  if (full)
    fprintf(stderr, "%s", q->opt);
  else
    fprintf(stderr, "%.*s", q->optlen, q->opt);
  if (suffix)
    fprintf(stderr, "%s", suffix);
}

/* This parses the current option and fills tino_getopt_impl.  It
 * fetches all the options, arguments and variable pointers from the
 * va_list, except for the default value in case TINO_GETOPT_DEFAULT
 * is used.
 */
static struct tino_getopt_impl *
tino_getopt_arg(struct tino_getopt_impl *p, TINO_VA_LIST list, const char *arg)
{
  if (!arg)
    arg	= TINO_VA_ARG(list, const char *);
#if 0
  p->unknown	= 0;
  p->help	= 0;
#endif
  p->opt	= 0;
  p->type	= (enum tino_getopt_type)0;
#if 0
  p->arg	= arg;
#endif
  if (!arg)
    return 0;

  /* Parse all the possible flags
   */
  for (;;)
    TINO_GETOPT_IFflg(DEBUG)
      TINO_GETOPT_IFflg(SUFFIX)
      TINO_GETOPT_IFflg(TIMESPEC)
      TINO_GETOPT_IFflg(IGNERR)
      TINO_GETOPT_IFflg(TAR)
      TINO_GETOPT_IFflg(POSIX)
      TINO_GETOPT_IFflg(PLUS)
      TINO_GETOPT_IFflg(LOPT)
      TINO_GETOPT_IFflg(LLOPT)
      TINO_GETOPT_IFflg(DIRECT)
      TINO_GETOPT_IFflg(DD)
      TINO_GETOPT_IFflg(USAGE)
      TINO_GETOPT_IFflg(DEFAULT)
      TINO_GETOPT_IFflg(NODEFAULT)
      TINO_GETOPT_IFptr(DEFAULT_PTR)
      TINO_GETOPT_IFtyp(HELP)
      TINO_GETOPT_IFptr(USER)
      TINO_GETOPT_IFptr(FN)
      TINO_GETOPT_IFptr(CB)
#if 0
      TINO_GETOPT_IFptr(VALID_STR)
#endif
      TINO_GETOPT_IFtyp(FLAG)
      TINO_GETOPT_IFtyp(STRINGFLAG)
      TINO_GETOPT_IFtyp(STRING)
      TINO_GETOPT_IFtyp(UCHAR)
      TINO_GETOPT_IFtyp(CHAR)
      TINO_GETOPT_IFtyp(UBYTE)
      TINO_GETOPT_IFtyp(BYTE)
      TINO_GETOPT_IFtyp(USHORT)
      TINO_GETOPT_IFtyp(SHORT)
      TINO_GETOPT_IFtyp(UNSIGNED)
      TINO_GETOPT_IFtyp(INT)
      TINO_GETOPT_IFtyp(ULONGINT)
      TINO_GETOPT_IFtyp(LONGINT)
      TINO_GETOPT_IFtyp(ULLONG)
      TINO_GETOPT_IFtyp(LLONG)
      TINO_GETOPT_IFflg(MIN)
      TINO_GETOPT_IFflg(MAX)
      TINO_GETOPT_IFptr(MIN_PTR)
      TINO_GETOPT_IFptr(MAX_PTR)
    {
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
#if 0
	      p->help	= arg-1;
#endif
	      if (p->DEBUG_var)
		fprintf(stderr, "getopt help: '%s'\n", arg);
	      /* If we hit the end of the string there is no help text
	       * (be graceful and accept it)
	       */
	    case 0:
	      p->optlen	= arg-p->opt-1;
	      if (p->DEBUG_var)
		fprintf(stderr, "getopt option: '%.*s'\n", p->optlen, p->opt);
	      /* Fetch the pointer to the variable
	       * As we do not know which variable this is, do it a generic way
	       */
	      if (p->type)
		TINO_GETOPT_IFarg(TINO_GETOPT_GENERIC_PTR);
	      return p;

	      /* We hit a \1 which means, we hit something we do not understand.
	       * p->opt points to the start of this crap.
	       * have grace: skip everything which is unknown
	       */
	    case '\1':
	      if (p->DEBUG_var)
		fprintf(stderr, "getopt unknown: '%.*s'\n", (int)(arg-p->opt), p->opt);
#if 0
	      if (!p->unknown)
		p->unknown	= p->opt;
#endif
	      break;
	    }
	  break;
	}
    }
}
#undef TINO_GETOPT_IFtyp
#undef TINO_GETOPT_IFptr
#undef TINO_GETOPT_IFflg
#undef TINO_GETOPT_IFarg
#undef TINO_GETOPT_IFgen

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
tino_getopt_var_to_str(const union tino_getopt_types *ptr, enum tino_getopt_type type, char auxbuf[TINO_GETOPT_AUXBUF_SIZE])
{
  switch (type)
    {
      long long			lld;
      unsigned long long	llu;
      int			i;

    case TINO_GETOPT_TYPE_STRINGFLAG:
    case TINO_GETOPT_TYPE_STRING:	return ptr->s;

    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      i	= ptr->C+1;
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
		   ptr->C,
		   ptr->C,
		   (auxbuf[i] && auxbuf[i]!='-' ? ' ' : 0),
		   auxbuf[i]);
	  break;
	}
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE,
	       "'%c' 0x%02x %d",
	       ptr->c,
	       ptr->C,
	       ptr->C);
      break;

    case TINO_GETOPT_TYPE_HELP:		strcpy(auxbuf, "(has no value)");	break;
    case TINO_GETOPT_TYPE_FLAG:		strcpy(auxbuf, ptr->u ? "SET(1)" : "UNSET(0)"); if (ptr->u<=1) break;
      /* fallthrough!	*/
    case TINO_GETOPT_TYPE_INT:		lld	= ptr->i;	if (0)
    case TINO_GETOPT_TYPE_BYTE:		lld	= ptr->c;	if (0)
    case TINO_GETOPT_TYPE_SHORT:	lld	= ptr->w;	if (0)
    case TINO_GETOPT_TYPE_LONGINT:	lld	= ptr->I;	if (0)
    case TINO_GETOPT_TYPE_LLONG:	lld	= ptr->l;
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE, "%lld", lld);
      break;

    case TINO_GETOPT_TYPE_UNSIGNED:	llu	= ptr->u;	if (0)
    case TINO_GETOPT_TYPE_UBYTE:	llu	= ptr->C;	if (0)
    case TINO_GETOPT_TYPE_USHORT:	llu	= ptr->W;	if (0)
    case TINO_GETOPT_TYPE_ULONGINT:	llu	= ptr->U;	if (0)
    case TINO_GETOPT_TYPE_ULLONG:	llu	= ptr->L;
      snprintf(auxbuf, TINO_GETOPT_AUXBUF_SIZE, "%llu", llu);
      break;
    }
  return auxbuf;
}

static void
tino_getopt_var_set_varg(struct tino_getopt_impl *p, union tino_getopt_types *ptr, TINO_VA_LIST list)
{
  if (!ptr)
    ptr	= p->varptr;
  if (p->DEBUG_var)
    {
      fprintf(stderr, "getopt set: %s %.*s to ",
	      (ptr==p->varptr ? "opt" :
	       ptr== &p->min ? "min" :
	       ptr== &p->max ? "max" :
	       "unknown(oops)"
	       ), p->optlen, p->opt);
      fflush(stderr);
    }
  switch (p->type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      ptr->i	= TINO_VA_ARG(list, int);
      break;

    case TINO_GETOPT_TYPE_STRINGFLAG:
    case TINO_GETOPT_TYPE_STRING:
      ptr->s	= TINO_VA_ARG(list, char *);
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      ptr->c	= TINO_VA_ARG(list, int);
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      ptr->w	= TINO_VA_ARG(list, int);
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      ptr->I	= TINO_VA_ARG(list, long);
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      ptr->l	= TINO_VA_ARG(list, long long);
      break;
	  
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
  if (p->DEBUG_var)
    {
      char	auxbuf[TINO_GETOPT_AUXBUF_SIZE];

      fprintf(stderr, "%s\n", tino_getopt_var_to_str(ptr, p->type, auxbuf));
    }
}

static void
tino_getopt_var_set_ptr(struct tino_getopt_impl *p, const union tino_getopt_types *ptr)
{
  char	auxbuf[TINO_GETOPT_AUXBUF_SIZE];

  if (p->DEBUG_var)
    fprintf(stderr, "getopt setting: opt %.*s to %s\n", p->optlen, p->opt, tino_getopt_var_to_str(ptr, p->type, auxbuf));
  switch (p->type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      p->varptr->i	= ptr->i;
      break;

    case TINO_GETOPT_TYPE_STRINGFLAG:
    case TINO_GETOPT_TYPE_STRING:
      p->varptr->s	= ptr->s;
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      p->varptr->c	= ptr->c;
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      p->varptr->w	= ptr->w;
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      p->varptr->I	= ptr->I;
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      p->varptr->l	= ptr->l;
      break;

    case TINO_GETOPT_TYPE_HELP:
      break;
    }
}

static void
tino_getopt_var_set_0(struct tino_getopt_impl *p)
{
  if (p->DEBUG_var)
    fprintf(stderr, "getopt nulling: opt %.*s\n", p->optlen, p->opt);
  switch (p->type)
    {
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_FLAG:
      p->varptr->i	= 0;
      break;

    case TINO_GETOPT_TYPE_STRINGFLAG:
    case TINO_GETOPT_TYPE_STRING:
      p->varptr->s	= 0;
      break;

    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      p->varptr->c	= 0;
      break;

    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_SHORT:
      p->varptr->w	= 0;
      break;

    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_LONGINT:
      p->varptr->I	= 0;
      break;

    case TINO_GETOPT_TYPE_ULLONG:
    case TINO_GETOPT_TYPE_LLONG:
      p->varptr->l	= 0;
      break;
	  
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
}

/* This is not ready yet
 */
static int
tino_getopt_var_set_arg_imp(struct tino_getopt_impl *p, const char *arg, int n)
{
  unsigned long long	ull;
  char			auxbuf[TINO_GETOPT_AUXBUF_SIZE];
  char			*end;

  switch (p->type)
    {
    case TINO_GETOPT_TYPE_HELP:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt debug: usage via %.*s\n", p->optlen, p->opt);
      return -1;

    case TINO_GETOPT_TYPE_FLAG:
      if (p->MIN_var && (
            !p->MAX_var ||
	      (p->min.i<=p->max.i ? (p->varptr->i<p->min.i || p->varptr->i>p->max.i) : (p->varptr->i>p->min.i || p->varptr->i<p->max.i)))
         )
	{
	  /* Jump value to MIN Value if needed
	   */
	  p->varptr->i	= p->min.i;
	}
      else if (p->MAX_var)
	{
	  /* Increment/Decremet to MAX value
	   */
	  if (p->max.i<p->varptr->i)
	    p->varptr->i--;
	  else if (p->max.i>p->varptr->i)
	    p->varptr->i++;
	}
      else
        {
	  /* Default action: Set to 1
	   */
	  p->varptr->i	= 1;
	}
      if (p->DEBUG_var)
	fprintf(stderr, "getopt set: flag %.*s to %d\n", p->optlen, p->opt, p->varptr->i);
      return 0;

    case TINO_GETOPT_TYPE_STRINGFLAG:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt set: stringflag %.*s to '%s'\n", p->optlen, p->opt, p->min.s);
      p->varptr->s	= p->MIN_PTR_var ? p->MIN_PTR_var->s : p->min.s;
      return 0;

    case TINO_GETOPT_TYPE_STRING:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt set: opt %.*s to string '%s'\n", p->optlen, p->opt, arg);
      p->varptr->s	= arg;
      return n;

    case TINO_GETOPT_TYPE_UCHAR:
    case TINO_GETOPT_TYPE_CHAR:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt set: opt %.*s to char '%s'\n", p->optlen, p->opt, arg);
      p->varptr->c	= *arg;
      if (*arg && arg[1])
	{
	  if (isdigit(arg[0]))
	    {
	      long	tmp;
	      char	*end;

	      tmp	= strtol(arg, &end, 0);
	      if (end && !*end && tmp>=0 && tmp<=255)
		p->varptr->c	= tmp;
	    }
	  else if (arg[0]=='\\')
	    switch (arg[1])
	      {
	      case '0':	p->varptr->c	= 0;	break;
	      case 'a':	p->varptr->c	= '\a';	break;
	      case 'b':	p->varptr->c	= '\b';	break;
	      case 'f':	p->varptr->c	= '\f';	break;
	      case 'n':	p->varptr->c	= '\n';	break;
	      case 'r':	p->varptr->c	= '\r';	break;
	      case 't':	p->varptr->c	= '\t';	break;
	      case 'v':	p->varptr->c	= '\v';	break;
	      default:
		p->varptr->c	= arg[1];
		break;
	      }
	}
      return n;

    default:
      break;
    }

  if (p->DEBUG_var)
    fprintf(stderr, "getopt set: opt %.*s (%s)\n", p->optlen, p->opt, arg);

  end	= 0;
  switch (p->type)
    {
    default:
      return -1;
      
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_ULLONG:
      ull	= strtoull(arg, &end, 0);
      break;

    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_SHORT:
    case TINO_GETOPT_TYPE_LONGINT:
    case TINO_GETOPT_TYPE_LLONG:
      ull	= strtoll(arg, &end, 0);
      break;
    }
  if (end && *end && p->SUFFIX_var)
    {
      unsigned long long	o;

      o	= ull;
      switch (*end++)
	{
	default:
	  fprintf(stderr, "getopt: option %.*s unknown suffix: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	  o	= 0;
	  break;

	case 't':	ull *= 1000ull;
	case 'g':	ull *= 1000ull;
	case 'm':	ull *= 1000ull;
	case 'k':	ull *= 1000ull;
	case 'b':	break;
	case 'T':	ull *= 1024ull;
	case 'G':	ull *= 1024ull;
	case 'M':	ull *= 1024ull;
	case 'K':	ull *= 1024ull;
	case 'B':	break;
	}
      if (o && o>ull)
	{
	  fprintf(stderr, "getopt: option %.*s overflow by suffix: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	}
    }
  if (end && *end && p->TIMESPEC_var)
    {
      unsigned long long	o;

      o	= ull;
      switch (*end++)
	{
	default:
	  fprintf(stderr, "getopt: option %.*s unknown timespec: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	  o	= 0;
	  break;

	  /* estimates rounded up	*/
	case 'C':	ull *= 36525ull; if (0)	/* Century	*/
	case 'D':	ull *= 3653ull; if (0)	/* Decade=10y	*/
	case 'Y':	ull *= 366ull; if (0)	/* Year		*/
	case 'S':	ull *= 92ull; if (0)	/* Season	*/
	case 'M':	ull *= 31ull; if (0)	/* Month	*/

	  /* exact	*/
	case 'w':	ull *= 7ull;		/* Week	*/
	case 'd':	ull *= 24ull;		/* day	*/
	case 'h':	ull *= 60ull;		/* hour	*/
	case 'm':	ull *= 60ull;		/* minute	*/
	case 's':	break;			/* seconds	*/
	}
      if (o && o>ull)
	{
	  fprintf(stderr, "getopt: option %.*s overflow by timespec: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	}
    }

  if (!end || *end)
    {
      fprintf(stderr, "getopt: option %.*s numeric value has unknown suffix: %s\n", p->optlen, p->opt, end);
      if (!p->IGNERR_var)
	return -3;
    }

  /* check if the type fits into the argument
   *
   * Negative values are permissible by unsigned data types, as it's
   * convenient to give -1 instead of 255 to a byte.
   */

#define	TINO_GETOPT_VAR_SET_ARG_CHECK(VAR,MIN,MAX)			\
      p->varptr->VAR	= ull;						\
      if (!((ull<(unsigned long long)(MIN) && ull>(MAX)) ||		\
            (p->MIN_var     && p->varptr->VAR<p->min.VAR) ||		\
	    (p->MAX_var     && p->varptr->VAR>p->max.VAR) ||		\
	    (p->MIN_PTR_var && p->varptr->VAR<p->MIN_PTR_var->VAR) ||	\
	    (p->MAX_PTR_var && p->varptr->VAR>p->MAX_PTR_var->VAR)))	\
	return n;							\
      break

#define TINO_GETOPT_VAR_SET_ARG_CHECK_U(VAR,TYPE)	\
	TINO_GETOPT_VAR_SET_ARG_CHECK(VAR,(TYPE)(~(((unsigned TYPE)-1)>>1)),(unsigned TYPE)-1)
#define TINO_GETOPT_VAR_SET_ARG_CHECK_S(VAR,TYPE)	\
	TINO_GETOPT_VAR_SET_ARG_CHECK(VAR,(TYPE)(~(((unsigned TYPE)-1)>>1)),((unsigned TYPE)-1)>>1)

  switch (p->type)
    {
    default:
      return -1;
      
    case TINO_GETOPT_TYPE_UNSIGNED:	TINO_GETOPT_VAR_SET_ARG_CHECK_U(u, int);
    case TINO_GETOPT_TYPE_INT:		TINO_GETOPT_VAR_SET_ARG_CHECK_S(i, int);
    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_UCHAR:	TINO_GETOPT_VAR_SET_ARG_CHECK_U(C, char);
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_CHAR:		TINO_GETOPT_VAR_SET_ARG_CHECK_S(c, char);
    case TINO_GETOPT_TYPE_USHORT:	TINO_GETOPT_VAR_SET_ARG_CHECK_U(W, short);
    case TINO_GETOPT_TYPE_SHORT:	TINO_GETOPT_VAR_SET_ARG_CHECK_S(w, short);
    case TINO_GETOPT_TYPE_ULONGINT:	TINO_GETOPT_VAR_SET_ARG_CHECK_U(U, long);
    case TINO_GETOPT_TYPE_LONGINT:	TINO_GETOPT_VAR_SET_ARG_CHECK_S(I, long);
    case TINO_GETOPT_TYPE_ULLONG:	TINO_GETOPT_VAR_SET_ARG_CHECK_U(L, long long);
    case TINO_GETOPT_TYPE_LLONG:	TINO_GETOPT_VAR_SET_ARG_CHECK_S(l, long long);
    }

  /* Out of bounds
   */
  switch (p->type)
    {
    default:
      return -1;
      
    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_ULLONG:
      fprintf(stderr, "getopt: value %llu out of bounds for option %.*s: %s\n", ull, p->optlen, p->opt, arg);
      break;

    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_SHORT:
    case TINO_GETOPT_TYPE_LONGINT:
    case TINO_GETOPT_TYPE_LLONG:
      fprintf(stderr, "getopt: value %lld out of bounds for option %.*s: %s\n", (long long)ull, p->optlen, p->opt, arg);
      break;
    }
  fprintf(stderr, "getopt: ranges");
  if (p->MIN_var)
    fprintf(stderr, " min=%s", tino_getopt_var_to_str(&p->min, p->type, auxbuf));
  if (p->MAX_var)
    fprintf(stderr, " max=%s", tino_getopt_var_to_str(&p->max, p->type, auxbuf));
  if (p->MIN_PTR_var)
    fprintf(stderr, " min*=%s", tino_getopt_var_to_str(p->MIN_PTR_var, p->type, auxbuf));
  if (p->MAX_PTR_var)
    fprintf(stderr, " max*=%s", tino_getopt_var_to_str(p->MAX_PTR_var, p->type, auxbuf));
  fprintf(stderr, "\n");
  return p->IGNERR_var ? n : -3;
}

static int
tino_getopt_var_set_arg(struct tino_getopt_impl *p, const char *arg, const char *next)
{
  int		n;
  const char	*s;

  n			= 1;
  p->DEFAULT_var	= 0;
  p->NODEFAULT_var	= 0;
  if (!arg || (!*arg && !p->DIRECT_var))
    {
      n		= 2;
      arg	= next;
      if (!arg)
	arg	= "";
    }
  else if ((p->LLOPT_var || p->LOPT_var || p->DD_var) && *arg)
    {
      /* Long options have --"long"=arg or --"long."arg
       */
      if (isalnum(p->opt[p->optlen-1]))
	arg++;
    }
  n	= tino_getopt_var_set_arg_imp(p, arg, n);
  if (n<0 || !p->FN_var)
    return n;

  s	= p->FN_var(p->varptr, arg, p->opt, p->USER_var);
  if (!s)
    return n;

  if (*s)
    fprintf(stderr, "getopt: rejected option %.*s: %s\n", p->optlen, p->opt, s);
  return -1;
}

#ifdef TINO_MAXOPTS
#error	"TINO_MAXOPTS now is called TINO_GETOPT_MAXOPTS!"
#endif
#ifndef TINO_GETOPT_MAXOPTS
#define	TINO_GETOPT_MAXOPTS	256	/* Yeah, I need to get rid of this sometimes	*/
#endif

/* Initialize the tino_getopt_impl array
 */
static int
tino_getopt_init(const char *global, TINO_VA_LIST list, struct tino_getopt_impl *q, int max)
{
  int	opts;

  /* Parse the first argument (global):
   * Version
   * Compilation date
   * Global options
   * Usage string
   */
  q[-1].opt	= global;
  tino_getopt_tab(global, &global);
  tino_getopt_tab(global, &global);
  tino_getopt_arg(q, list, global);

  /* Parse all the possible arguments into an array
   */
  for (opts=0; ++opts<max; )
    {
      /* copy global settings
       */
      q[opts]	= q[0];
      if (!tino_getopt_arg(q+opts, list, NULL))
	break;
      /* Preset the variables
       */
      if (q[opts].DEFAULT_PTR_var)
	tino_getopt_var_set_ptr(q+opts, q[opts].DEFAULT_PTR_var);
      else if (q[opts].NODEFAULT_var)
	{
	  if (q[opts].DEFAULT_var)
	    {
	      /* If TINO_GETOPT_DEFAULT is set globally, this warning
	       * would make no sense, as you can only suppress default
	       * values using TINO_GETOPT_NODEFAULT.
	       */
	      if (!q[0].DEFAULT_var && q[opts].DEBUG_var)
		fprintf(stderr, "getopt: TINO_GETOPT_DEFAULT ignored on option %.*s, check parameters!\n", q[opts].optlen, q[opts].opt);
#if 0
	      tino_getopt_var_set_varg(q+opts, &dummy, list);
#endif
	    }
	}
      else if (q[opts].DEFAULT_var)
	tino_getopt_var_set_varg(q+opts, q[opts].varptr, list);
      else
	tino_getopt_var_set_0(q+opts);

      /* Get MIN and MAX (MIN_PTR and MAX_PTR already fetched)
       */
      if (q[opts].MIN_var)
	tino_getopt_var_set_varg(q+opts, &q[opts].min, list);
      if (q[opts].MAX_var)
	tino_getopt_var_set_varg(q+opts, &q[opts].max, list);

      /* for below
       */
      q[-1].TAR_var	|= q[opts].TAR_var;
      q[-1].POSIX_var	|= q[opts].POSIX_var;
      q[-1].PLUS_var	|= q[opts].PLUS_var;
      q[-1].DD_var	|= q[opts].DD_var;
      q[-1].LOPT_var	|= q[opts].LOPT_var;
      q[-1].LLOPT_var	|= q[opts].LLOPT_var;
    }

  if (opts==max)
    fprintf(stderr, "getopt: too many builtin options, increase TINO_GETOPT_MAXOPTS (=%d), continuing anyway\n", TINO_GETOPT_MAXOPTS);

  return opts;
}

/* Parse the command line
 *
 * Returns -1 for help option, -2 for general error, -3 for range
 * error (min..max), else >0
 */
static int
tino_getopt_parse(int argc, char **argv, struct tino_getopt_impl *q, int opts)
{
  int	pos, i;

  if (q[0].TAR_var || q[0].POSIX_var || q[0].PLUS_var)
    fprintf(stderr, "getopt: tar/posix/plus not yet implemented, continuing anyway\n");

  if (q[0].TAR_var && argc>1 && argv[1][0]!='-')
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
  for (pos=0; ++pos<argc; )
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
	      if (!*++ptr && (q[0].POSIX_var || q[0].PLUS_var || q[0].LOPT_var || q[0].LLOPT_var || !(q[0].DD_var || q[0].TAR_var)))
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
		      return -2;
		    }

#define	TINO_GETOPT_CMPLONGOPT(I)	(!q[I].optlen ||			\
					 strncmp(ptr, q[I].opt, q[I].optlen) ||	\
					 (ptr[q[I].optlen]			\
					  && ptr[q[I].optlen]!='='		\
					  && isalnum(q[I].opt[q[I].optlen-1])	\
					  )					\
					 )

#define	TINO_GETOPT_PROCESSLONGOPT(I,COND)						\
	      if (!(COND) || TINO_GETOPT_CMPLONGOPT(I))					\
		continue;								\
	      ptr	+= q[I].optlen;							\
	      I		= tino_getopt_var_set_arg(q+I, ptr, argv[pos+1]);		\
	      if (!I && *ptr)								\
		{									\
		  fprintf(stderr, "getopt: flag %s must not have args\n", argv[pos]);	\
		  return -2;								\
		}
              /* i<0	help option or error
	       * i==0	last thing was flag
	       * i==1	last thing was argument
	       * i==2	one addional argv was eaten away
	       */
	  
		  TINO_GETOPT_PROCESSLONGOPT(i,q[i].LLOPT_var);
		  break;
		}
	      /* The option has been processed successfully if i>=0
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
			  return -2;
			}
		    } while (((q[i].LLOPT_var || q[i].DD_var) && !q[i].LOPT_var) ||
			     !q[i].optlen || strncmp(ptr, q[i].opt, q[i].optlen));
		  ptr	+= q[i].optlen;
		  i	= tino_getopt_var_set_arg(q+i, ptr, argv[pos+1]);
		  if (i)
		    break;
		} while (*ptr);
	      /* All options have been processed successfully if i>=0
	       */
	    }
	  /* short or long option
	   * i<0	help option or error
	   * i==0	last thing was flag
	   * i==1	last thing was argument
	   * i==2	one addional argv was eaten away
	   */
	  if (i<0)
	    return i;

	  if (i>1)
	    pos++;
	  continue;
	}

      /* hunt for DD like options
       */
      if (q[0].DD_var)
	{
	  for (i=opts; --i>1; )
	    {
	      TINO_GETOPT_PROCESSLONGOPT(i,q[i].DD_var);
              /* i<0	help option or error
	       * i==0	last thing was flag
	       * i==1	last thing was argument
	       * i==2	one addional argv was eaten away
	       */
	      if (i<0)
		return i;
	      if (i>1)
		pos++;
	      i	= 1;
	      break;
	    }
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
      if (q[1].CB_var && (i=(q[1].CB_var)(pos, argv, argc, q[1].USER_var))!=0)
	{
	  if (i<0)
	    return i;
	  pos	+= i;
	  continue;
	}
      if (!q[0].POSIX_var)
	break;

      /* Not yet implemented
       * reorder the options (POSIX) ..
       */
      TINO_XXX;
      break;
    }

  /* Successfully parsed, pos>0
   */
  return pos;
}

/* Format the usage (print help)
 */
static void
tino_getopt_usage(char **argv, struct tino_getopt_impl *q, int opts, int help)
{
  const char	*arg0, *usage, *date, *rest;
  int		i, optlen, datelen;

  optlen	= tino_getopt_tab(q[-1].opt, &date);
  datelen	= tino_getopt_tab(date, &rest);

  usage	= q[0].opt;
#ifndef TINO_GETOPT_USAGE_FULL_PATH
  if ((arg0=strrchr(argv[0], '/'))!=0 ||	/* Unix	*/
      (arg0=strrchr(argv[0], '\\'))!=0)		/* Windows, sigh	*/
    arg0++;
  else
#endif
    arg0	= argv[0];

  if (help!=-1)	/* Not usage requested	*/
    for (help=opts; --help>=1; )
      if (q[help].type==TINO_GETOPT_TYPE_HELP)
	break;
  if (help>0)
    {
      /* Print only a short usage if not requested.
       * Only possible if there is a usage option.
       */
      fprintf(stderr, "%s: for help try option ", arg0);
      tino_getopt_print_option_name(q+help, 0, "\n");
      return;
    }

  fprintf(stderr,
	  "Usage: %s [options]%.*s\n"
	  "\t\tversion %.*s compiled %.*s"
	  , arg0, tino_getopt_tab(usage, &usage), q[0].opt,
	  optlen, q[-1].opt,
	  datelen, date);
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
  for (i=0; ++i<opts; )
    {
      char		auxbuf[TINO_GETOPT_AUXBUF_SIZE];
      const char	*s;
      int		j;

      fputc('\t', stderr);
      tino_getopt_print_option_name(q+i, 1, "\n");
      fprintf(stderr, "%s\n", q[i].opt);
      s	= "\t\t(";
      for (j=0; ++j<opts; )
        if (j!=i && q[j].varptr==q[i].varptr)
 	  {
	    fprintf(stderr, "opt ");
	    tino_getopt_print_option_name(q+j, 0, ": ");
	  }
      if (q[i].NODEFAULT_var || q[i].DEFAULT_var)
	{
	  fprintf(stderr, "%sdefault ", s);
	  s	= tino_getopt_var_to_str(q[i].varptr, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (q[i].MIN_var)
	{
	  fprintf(stderr, "%s%s ", s, (q[i].type==TINO_GETOPT_TYPE_STRINGFLAG ? "set to" : "from"));
	  s	= tino_getopt_var_to_str(&q[i].min, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (q[i].MIN_PTR_var)
	{
	  fprintf(stderr, "%s%s* ", s, (q[i].type==TINO_GETOPT_TYPE_STRINGFLAG ? "set to" : "from"));
	  s	= tino_getopt_var_to_str(q[i].MIN_PTR_var, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (q[i].MAX_var)
	{
	  fprintf(stderr, "%sto ", s);
	  s	= tino_getopt_var_to_str(&q[i].max, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (q[i].MAX_PTR_var)
	{
	  fprintf(stderr, "%sto* ", s);
	  s	= tino_getopt_var_to_str(q[i].MAX_PTR_var, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (*s==' ')
	fprintf(stderr, ")\n");
    }
}

/* returns:
 * - Offset in the arguments where the non-options start.
 * - or 0 on usage or other things which are no real error.
 * - or -1 on error.
 */
static int
tino_getopt_hook(int argc, char **argv, int min, int max,
		 const char *global, TINO_VA_LIST list,
		 int (*hook)(struct tino_getopt_impl *, int max, void *user),
		 void *user)
{
  struct tino_getopt_impl	q[TINO_GETOPT_MAXOPTS];
  int				opts, pos;

  /* q[0] are calculated flags
   * q[1] are the defaults
   * q[2..] are the options to parse
   *
   * The routines expect to have q[0] the defaults,
   * such that q[-1] are the calculated flags
   */
  memset(q,0,sizeof q);

  /* Parse the global string into the array
   */
  opts	= tino_getopt_init(global, list, q+1, (sizeof q/sizeof *q)-1);

  /* Run the hook if one is set.
   *
   * This is used to run the inifile, or whatever you want
   */
  if (hook && hook(q+1, opts, user))
    return -1;

  /* Parse the commandline arguments according to the options
   */
  pos	= tino_getopt_parse(argc, argv, q+1, opts);

  /* If number of arguments are ok, just return the offset.
   */
  if (pos>0)
    {
      if (argc-pos>=min && (max<min || argc-pos<=max))
	return pos;
      if (argc-pos<min)
	fprintf(stderr, "getopt: missing arg(s)\n");
      else
	fprintf(stderr, "getopt: too many args\n");
    }

  tino_getopt_usage(argv, q+1, opts, pos);
  return 0;	/* usage printed or other error	*/
}

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
  tino_va_list	list;
  int		ret;

  tino_va_start(list, global);
  ret	= tino_getopt_hook(argc, argv, min, max, global, &list, NULL, NULL);
  tino_va_end(list);
  return ret;
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
  int		argn, flag, i, v;
  long		t;
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
		      TINO_GETOPT_DEFAULT
		      TINO_GETOPT_MIN
		      TINO_GETOPT_MAX
		      "f	set a flag"
		      , &flag,
		      10,
		      20,
		      30,

		      TINO_GETOPT_INT
		      TINO_GETOPT_DEFAULT
		      TINO_GETOPT_MIN
		      TINO_GETOPT_MAX
		      TINO_GETOPT_SUFFIX
		      "n nr	number (kmgt-suffix)"
		      , &i,
		      50,
		      0,
		      10000,

		      TINO_GETOPT_LONGINT
		      TINO_GETOPT_TIMESPEC
		      "t time	number (mhdw-suffix)"
		      , &t,

		      TINO_GETOPT_FLAG
		      TINO_GETOPT_MIN
		      TINO_GETOPT_MAX
		      "q	be more quiet"
		      , &v,
		      10,
		      -10,

		      TINO_GETOPT_FLAG
		      TINO_GETOPT_MIN
		      TINO_GETOPT_MAX
		      "v	be more verbose"
		      , &v,
		      -10,
		      10,

		      NULL
		      );
  if (argn<=0)
    return 1;
  
  printf("argc:    %d\n", argc);
  printf("argn:    %d\n", argn);
  printf("verbose: %d\n", v);
  printf("time:    %ld\n", t);
  printf("string:  %s\n", str);
  printf("flag:    %d\n", flag);
  printf("int:     %d\n", i);
  printf("char:    %s\n", tino_getopt_var_to_str(&c, TINO_GETOPT_TYPE_CHAR, auxbuf));
  for (; argn<argc; argn++)
    printf("arg%03d:  %s\n", argn, argv[argn]);
  return 0;
}
#endif

#endif
