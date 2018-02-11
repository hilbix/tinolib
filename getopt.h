/* *IMPORTANT*: If you have trouble using it, try to set
 * TINO_GETOPT_DEBUG as seen in the example at the end!
 *
 * Many features are still missing, however I regularily use it.  And
 * beware, some option combinations are rarely tested!
 *
 * My way of doing "the swiss army knife of" getopt:
 *
 * This differs from ordinary getopt in several ways:
 * - It forces you to have a version.
 * - It implicitly prints the usage, too.
 * - It is easy to use and easy to read (noncryptic).
 * - Parses all parameters from one single function call.
 * - It does not handle long and short options differently.
 * - It will not parse options everywhere on the command line.
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
 * For an example, see at the end!
 *
 * Apparently you are not allowed to use \1 in your strings.  ;)
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
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

/* IF YOU ARE PUZZLED (at least I am *eg*):
 *
 * This only seems complex, have a look at the example at the end.  It
 * may be a little bit more complex to write than usual (complex does
 * not mean slower!), but after some years it is quite handy.  Then it
 * still is easy to read and maintain, and you will even more easily
 * understand what's going on, even if some complex inner logic is
 * present (as possible with _MIN_PTR and _MAX_PTR).
 *
 * The good thing is, that you can encapsulate nearly everything you
 * need to express in one single *readable* function call to have your
 * arguments parsed and initialized.  And to add new options later on
 * just becomes a breeze.
 *
 * FLAG is something which starts with TINO_GETOPT_
 *
 * Zeroth: ONLY ADD COMMAS WHEN NOTED HERE.  Use
 * TINO_GETOPT_VERSION(xxx), then optionally add global FLAGs, then
 * add a string starting with a SPC and the arguments short explain
 * (as usually done in usages).  Add more strings starting with LF and
 * TAB for more usage information.  Then add a COMMA.
 *
 * First: Give the FLAGs TINO_GETOPT_* to your option.  This includes
 * the Type of the variable.
 *
 * Second: Add the SPC separated option string with arguments to the
 * option
 *
 * Third: Add a string starting with a TAB and then what explains the
 * option.  Add LF where needed, do not add an LF at the end.
 *
 * Fourth: In the sequence of the FLAG (see below): Add a COMMA and
 * the parameter to the FLAGs.  NOT ALL FLAGs NEED THIS.
 *
 * Fifth: Add a COMMA, the pointer to the variable to set (according
 * to TINO_GETOPT_type, see below) and add another COMMA.
 *
 * Sixth: For the options TINO_GETOPT_DEFAULT, TINO_GETOPT_MIN,
 * TINO_GETOPT_MAX (in this sequence) add the value and a comma.
 *
 * Seventh: At the end, add a NULL.
 *
 * TINO_GETOPT_type is explained in the section "Data type of options"
 * There you can find, what pointer is expected.  The pointer must be
 * to the correct data type, else strange things can happen (as always
 * in C).
 *
 * The second exception to FLAGs which do not immediately fetch
 * arguments are _DEFAULT, _MIN, and _MAX (_MIN_PTR and _MAX_PTR do
 * *not* belong to this).  Those fetch their values after the variable
 * has been initialized (because they must look into the variable
 * value).
 *
 * So BEST PRACTICE is to FIRST give all the FLAGs which fetch or
 * alter, THEN the data type, THEN _DEFAULT, _MIN, _MAX, to have the
 * FLAGs in the same order as the arguments fetched behind the option
 * string.  However this is not required by the function, as _type,
 * _DEFAULT, _MIN and _MAX are fetched delayed.
 *
 * Also BEST PRATICE is to have the options sorted ABC to keep
 * oversight.  However the Usage printed is in the same order, so
 * usually I add _HELP at the beginning, as this is already printed
 * when argument parsing fails.
 *
 * IF YOU ARE IN TROUBLE:
 *
 * At step Zeroth add TINO_GETOPT_DEBUG (see example at the end).
 * Also at the end of the arguments to the function add some more
 * COMMA NULL (, NULL) sequences to catch synchronization trouble.  If
 * you have fixed things, remove both again.
 */

/** Always start the global arg to GETOPT with this string!
 */
#define TINO_GETOPT_VERSION(VERSION)	VERSION "\t" __DATE__ "\t"

/** Wrapper for platforms where pointer layout of (union *) is
 * incompatible to (int *) or similar.  Only needed for C99
 * compatibility on irregular platforms.
 */
#define	TINO_GETOPT_PTR(X)	((union tino_getopt_types *)(X))


/**********************************************************************/
/* Parsing options:
 */
#define TINO_GETOPT_DEBUG	"debug\1"	/* prefix to debug	*/

/** Global flags, just concatenate them like:
 * TINO_GETOPT_VERSION("vers") TINO_GETOPT_TAR ...
 *
 * Follow this with a TAB and the global command line usage string.
 *
 * Examples of the various forms:
 *
 * TAR "tar xCfz dir file args" but "tar -xCdir -ffile -z args"
 * _TAR NOT YET IMPLEMENTED!
 *
 * POSIX "tar args -x", without POSIX this is parsed as
 * "tar -- args -x", so -x becomes an argument.
 * _POSIX NOT YET IMPLEMENTED!
 *
 * PLUS "cmp -s" (sets silent) vs. "cmp +s" (clears silent)
 * Note: + can be behind the _FLAG, so -s+ and --longopt+
 * Note that + is similar to -- not -, so +sv means --sv+.  Perhaps
 * best this is combined with LOPT.
 *
 * LLOPT allows to write the form "--file=path" or "--file path".
 *
 * LOPT allows long and DD type options with - prefix, this is magic
 * and normally is not needed.  This can have deadly abiguities, if
 * you have non unique prefixes like "s" and "str".  Such ambiguities
 * are resolved in random order and might not be what you want.
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
 * "long arg" (set DIRECT to disable the latter).  You probably should
 * use this with getopt(argc,argv,0,0, ...), else there can be deadly
 * ambiguities to other program arguments.
 *
 * Note that you can parse commandlines in a stateful manner:
 *	cat -crlf file1 +crlf file2
 * where cr2crlf etc. are options to how the file must be processed.
 * For this there is TINO_GETOPT_CB, which will get passed file1 and
 * file2, the options are set as appropriate. (Feature untested so
 * far and is deadly incomplete.)
 *
 * If you found a regular argument processing which cannot be
 * formulated here (heuristical commandline parsers like that Fortran
 * compilers do do not count!) please drop me a note.  ;)
 */
#define TINO_GETOPT_TAR		"tar\1"	/* first arg is options like in TAR, NOT YET IMPLEMENTED */
#define TINO_GETOPT_POSIX	"posix\1"/* search all args for options, NOT YET IMPLEMENTED	*/
#define TINO_GETOPT_PLUS	"plus\1"/* allow +option, too (invert flags) */
#define TINO_GETOPT_LOPT	"lo\1"	/* allow long options with -	*/
#define TINO_GETOPT_LLOPT	"llo\1"	/* parse long options with --	*/
#define TINO_GETOPT_DIRECT	"d\1"	/* arg follows option directly	*/
#define TINO_GETOPT_DD		"dd\1"	/* dd like, no prefixes at all, always long */


/**********************************************************************/
/* Options to data types.
 *
 * If this is present, it alters the behavior
 * and/or fetches some additional data.
 */

/** This is the "usage" option flag.
 *
 * The standard is to have a code fragment like
 *
 *	TINO_GETOPT_USAGE
 *	"h	this help"
 *
 * or similar to name the option which prints out the full usage.  As
 * designed it cannot be combined with any other type than
 * TINO_GETOPT_HELP (for more information see there).
 *
 * Actually this flag here is not needed, so you could leave it away,
 * but perhaps future implementations will need it.  This sets the
 * USAGE_var, which is ignored in the current implementation.
 *
 * Actually this only acts as a not needed placeholder string.
 * However when you use it like noted above you can see what this
 * option does and it also looks nice in hexdumps of the binary, too.
 */
#define TINO_GETOPT_USAGE	"usage\1"

/** Fetch an user pointer.
 * Usually used in global.  Locally it overwrites only on one time.
 *
 * The user pointer is passed to parser functions.
 *
 * Fetches arg:
 *	void *
 */
#define TINO_GETOPT_USER	"user\1"

/** Fetch non-option processing callback.
 *
 * If you set this, this gets called with everything tino_getopt
 * cannot process as an option (which will make up an argument to the
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

/** Fetch argument processing function.
 *
 * Usually used in global.  Locally it overwrites only the single
 * option.
 *
 * The function will get:
 *	The pointer of the argument to set, already set to the value.
 *	The pointer to the interpreted value somewhere within argv[]
 *	The pointer to the option string (from the definition)
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

/** Set parameter default value.
 *
 * With _NODEFAULT, the variables will not be initialized (takes
 * precedence).  With _DEFAULT a default value will be fetched AFTER
 * the variable and all other options (so it's always last!).  The
 * default depends on the size of the variable, so beware of long long
 * (use 0ll) and pointers (use NULL, not 0)!
 *
 * IMPORTANT: The _DEFAULT value must be given *behind* the variable
 * pointer, else you will get segmentation violation (optionally
 * followed by _MIN and _MAX values)!  This is because the type of the
 * variable to fetch is only known after everything else has been
 * parsed.  However this is not true for the _PTR and _ENV values, as
 * pointers lengths are always known in advance (assuming that the
 * pointers are compatible with the (void *) type).  Those must be
 * given *before* the variable pointer in the sequence of other
 * pointers like _PTR.
 *
 * Fetches arg:
 *
 * _ENV:	const char *
 *
 * _PTR:	A gerneric pointer to the variable, see
 *		TINO_GETOPT_PTR()
 *
 * _DEFAULT:	A type of the given variable.  If is fetched *behind*
 *		the variable pointer.
 *
 * Implementation note:
 *
 * Values from _ENV override all other defaults if the environment
 * variable exists.  However if the _ENV variable is missing, the
 * other defaults are used.
 *
 * You can give _DEFAULT globally, such that all parameters must have
 * default values.  This is true for all settings, though (however
 * it's not always useful).
 *
 * _PTR (if not NULL) takes precedence over _NODEFAULT and _DEFAULT
 * (but not _ENV).
 *
 * If _NODEFAULT is present _DEFAULT is ignored, in this case do not
 * try to give a default value even if you gave this option!
 *
 * If you are concerned about C99 compatibility on certain platforms,
 * you must wrap the _PTR args with the TINO_GETOPT_PTR() macro.  On
 * "usual" platforms this is not needed.
 */
#define	TINO_GETOPT_NODEFAULT	"keep\1"/* do not init variable	*/
#define	TINO_GETOPT_DEFAULT	"def\1"	/* give variable defaults */
#define	TINO_GETOPT_DEFAULT_PTR	"DEF\1"	/* pointer to default */
#define TINO_GETOPT_DEFAULT_ENV	"env\1"	/* take default from env-var */

/** Ignore errors while parsing following options to data types:
 */
#define TINO_GETOPT_IGNERR	"ign\1"

/** Min and Max parameters.
 * Must be followed by a Numeric Type from above.
 *
 * Fetches arg:
 *
 * _PTR:	A gerneric pointer to the variable, see
 *		TINO_GETOPT_PTR()
 *
 * _MIN _MAX:	A type of the given variable.  If is fetched *behind*
 *		the variable pointer *behind* (the optional) _DEFAULT
 *		value.  The sequence is _MIN then _MAX (if both are
 *		present).
 *
 * The _PTR version uses a pointer arg which points to the data.  This
 * pointer is read immediately when this option is requested!
 *
 * IMPORTANT: The argument for _MIN, _MAX must be given AFTER the
 * variable pointer, in the sequence _DEFAULT, _MIN, _MAX, else you
 * will get segmentation violation!  THIS IS NOT TRUE FOR THE _PTR
 * CASE!  The _PTR values must be given *before* the variable pointer
 * in the sequence they are present as option.
 *
 * If _MIN and _MIN_PTR are given (or _MAX and _MAX_PTR) then the
 * tighter values take precedence.  Note that you cannot repeat _MIN
 * nor _MAX.
 *
 * Not all possible _MIN and _MAX combinations are already
 * implemented, but most common cases shall work as expected.
 *
 * If you are concerned about C99 compatibility on certain platforms,
 * you must wrap the _PTR args with the TINO_GETOPT_PTR() macro.
 */
#define	TINO_GETOPT_MIN		"min"
#define	TINO_GETOPT_MAX		"max"
#define	TINO_GETOPT_MIN_PTR	"MIN"
#define	TINO_GETOPT_MAX_PTR	"MAX"

/** Allow bkmgt suffixes to numbers
 */
#define TINO_GETOPT_SUFFIX	"bkmgt\1"

/** Allow smhdw suffixes to numbers as time offsets
 */
#define TINO_GETOPT_TIMESPEC	"smhdw\1"

/** Increment a counter if option is processed.
 *
 * This enables you to check if an option is only used once, or you
 * can see if some options out of an option group using the same
 * counter were used.
 *
 * Fetches:
 *	int *
 */
#define TINO_GETOPT_COUNT	"count\1"


/**********************************************************************/
/* Data type of options.
 * Give them and concatenate options if needed.
 * These are mutually exclusive
 * (the last one wins, but don't depend on this).
 *
 * Follow this by the option string, a TAB, the usage string
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
 * Stringflags are used for multiple choice options.  Use _STRINGFLAGS
 * (it is only a convenience) for all options except the one which
 * sets the _DEFAULT value, for this use _STRINGFLAG.  For _STRINGFLAG
 * you must add the _DEFAULT value, else it defaults to NULL (as
 * always).  If you do not want to NULL the stringflag at all, use
 * _STRINGFLAGS for all options.  If you use _STRINGFLAG more than
 * once the behavior is undefined (the last _DEFAULT wins, but this
 * might change).
 *
 * Needs a pointer to:
 *	const char *
 * If option present:
 *	Pointer is set to the option string, which is \1 terminated!
 * If you want to set to another string:
 *	Give value as _MIN, see below for _MAX
 *
 * _MIN gives the flag value.  _MIN_PTR takes precedence over _MIN.
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

/** A string argument.
 *
 * Needs a pointer to:
 *	const char *
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

/** Byte flags, eat just one character.  Shoot me, but "-c1x" does
 * *not* mean "-c1 -x", it just ignores the x.
 */
#define TINO_GETOPT_UCHAR	"C\1"	/* unsigned character	*/
#define TINO_GETOPT_CHAR	"c\1"	/* character	*/

/** Numeric types (default to 0).
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

/** Ignore option
 *
 * This can be used to disable an option (must be the last type!) or
 * to just invoke the function if this option is encountered.
 */
#define	TINO_GETOPT_IGNORE	"_\1"	/* ignored type	*/

/** A "help" option.
 *
 * When this option is missing in the getopt options, the full usage
 * is printed on any commandline error encountered.  To suppress this
 * behavior and have a short usage on errors and a long one only if
 * requested, you need at least one such "help" option.
 *
 * TINO_GETOPT_HELP is the default option type, so you do not need to
 * specify it ever.  It is automatically active if not overwritten by
 * another type.  Therefor you do not need to give it with
 * TINO_GETOPT_USAGE, too.
 */
#define TINO_GETOPT_HELP	"help\1"


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
    TINO_GETOPT_TYPE_IGNORE=-1,
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
    /* ... */
#ifdef	TINO_GETOPT_EXT
    TINO_GETOPT_TYPE_EXT,
#endif
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
typedef const char		tino_getopt_DEFAULT_ENV;
typedef	void			tino_getopt_USER;
typedef const char *		tino_getopt_FN(void *, const char *, const char *, void *);
typedef int			tino_getopt_CB(int, char **, int, void *);
typedef int			tino_getopt_COUNT;

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
    tino_getopt_DEFAULT_ENV	*DEFAULT_ENV_var;

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
    tino_getopt_COUNT	*COUNT_var;
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
  p->type	= TINO_GETOPT_TYPE_HELP;
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
      TINO_GETOPT_IFptr(DEFAULT_ENV)
      TINO_GETOPT_IFtyp(HELP)	/* Note: You probably never need this	*/
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
      TINO_GETOPT_IFptr(COUNT)
      TINO_GETOPT_IFtyp(IGNORE)
#ifdef	TINO_GETOPT_EXT
      TINO_GETOPT_IFtyp(EXT)
#endif
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
	      /* fallthrough	*/
	    case 0:
	      p->optlen	= arg-p->opt-1;
	      if (p->DEBUG_var)
		fprintf(stderr, "getopt option: '%.*s'\n", p->optlen, p->opt);
	      /* Fetch the pointer to the variable
	       * As we do not know which variable this is, do it a generic way
	       */
	      if (p->type>TINO_GETOPT_TYPE_HELP)
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
		   &
		   "DELNULSOHSTXETXEOTENQACKBELBS\0HT\0LF\0VT\0FF\0CR\0SO\0SI\0"
		   "DLEDC1DC2DC3DC4NAKSYNETBCANEM\0SUBESCFS\0GS\0RS\0US\0"
		   [i*3],
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

    case TINO_GETOPT_TYPE_IGNORE:	strcpy(auxbuf, "(has no value)");	break;
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
    case TINO_GETOPT_TYPE_IGNORE:
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

    case TINO_GETOPT_TYPE_IGNORE:
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

    case TINO_GETOPT_TYPE_IGNORE:
    case TINO_GETOPT_TYPE_HELP:
      break;
    }
}

static int *
tino_getopt_flag_set_minmax(int is, int *a, int *b)
{
  if (b)
    return b;
  if (is && a)
    return a;
  return 0;
}

static int
tino_getopt_flag_val(struct tino_getopt_impl *p, int invert)
{
  int	*min, *max, def, val;

  min	= tino_getopt_flag_set_minmax(p->MIN_var, &p->min.i, &p->MIN_PTR_var->i);
  max	= tino_getopt_flag_set_minmax(p->MAX_var, &p->max.i, &p->MAX_PTR_var->i);
  def	= 1;
  if (invert)
    {
      int	*tmp;

      def	= 0;
      tmp	= min;
      min	= max;
      max	= tmp;
    }
  if (min && (
	      !max ||
	      (*min <= *max ? (p->varptr->i < *min || p->varptr->i > *max) : (p->varptr->i > *min || p->varptr->i < *max))
	      )
      )
    {
      /* Jump value to MIN Value if needed
       */
      return *min;
    }

  /* Set to default value if neither min nor max given	*/
  if (!max || invert)
    return def;	/* Set to default value	*/

  val	= p->varptr->i;
  /* Increment/Decremet to MAX value
   */
  if (*max < val)
    val--;
  else if (*max > val)
    val++;

  return val;
}

/* This is not complete yet
 *
 * Set a variable.
 *
 * Returns:
 * 0	option is processed, no arg was eaten
 * n	eaten the arg (n is from the parameter list)
 * -1	help requested
 * -2	internal error
 * -3	argument error (only if IGNERR is not set on option)
 */
static int
tino_getopt_var_set_arg_imp(struct tino_getopt_impl *p, const char *arg, int n, int invert)
{
  unsigned long long	ull;
  char			auxbuf[TINO_GETOPT_AUXBUF_SIZE];
  char			*end;
  int			is_signed;

  if (p->COUNT_var)
    {
      (*p->COUNT_var)++;
      if (p->DEBUG_var)
	fprintf(stderr, "getopt setting: count %p now %d\n", p->COUNT_var, *p->COUNT_var);
    }
  switch (p->type)
    {
    case TINO_GETOPT_TYPE_IGNORE:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt debug: ignored via %.*s\n", p->optlen, p->opt);
      return 0;

    case TINO_GETOPT_TYPE_HELP:
      if (p->DEBUG_var)
	fprintf(stderr, "getopt debug: usage via %.*s\n", p->optlen, p->opt);
      return -1;

    case TINO_GETOPT_TYPE_FLAG:
      p->varptr->i	= tino_getopt_flag_val(p, invert);
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
      TINO_XXX;	/* Check for argument overflow?	*/
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
      return -2;

    case TINO_GETOPT_TYPE_UNSIGNED:
    case TINO_GETOPT_TYPE_UBYTE:
    case TINO_GETOPT_TYPE_USHORT:
    case TINO_GETOPT_TYPE_ULONGINT:
    case TINO_GETOPT_TYPE_ULLONG:
      ull	= strtoull(arg, &end, 0);
      is_signed	= 0;
      break;

    case TINO_GETOPT_TYPE_INT:
    case TINO_GETOPT_TYPE_BYTE:
    case TINO_GETOPT_TYPE_SHORT:
    case TINO_GETOPT_TYPE_LONGINT:
    case TINO_GETOPT_TYPE_LLONG:
      ull	= strtoll(arg, &end, 0);
      is_signed	= 1;
      break;
    }
  if (end && *end && p->SUFFIX_var)
    {
      unsigned long long	o, f;

      f	= 1;
      switch (*end++)
	{
	default:
	  fprintf(stderr, "getopt: option %.*s unknown suffix: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	  break;

	case 'y':	f *= 1000ull;	/* 10^24 yotta	*/
	case 'z':	f *= 1000ull;	/* 10^21 zetta	*/
	case 'e':	f *= 1000ull;	/* 10^18 exa	*/
	case 'p':	f *= 1000ull;	/* 10^15 peta	*/
	case 't':	f *= 1000ull;	/* 10^12 tera	*/
	case 'g':	f *= 1000ull;	/* 10^9  giga	*/
	case 'm':	f *= 1000ull;	/* 10^6  mega	*/
	case 'k':	f *= 10ull;	/* 10^3  kilo	*/
	case 'h':	f *= 10ull;	/* 10^2  hecto	*/
	case 'd':	f *= 10ull;	/* 10^1  deka	*/
	case 'b':	break;
	case 'Y':	f *= 1024ull;	/* 8 Yotta, otto (it.)	*/
	case 'Z':	f *= 1024ull;	/* 7 Zetta, sette (it.)	*/
	case 'E':	f *= 1024ull;	/* 6 Exa, Hexagon	*/
	case 'P':	f *= 1024ull;	/* 5 Peta, Pentagon	*/
	case 'T':	f *= 1024ull;	/* 4 Tera, Tetraeder	*/
	case 'G':	f *= 1024ull;	/* 3 Giga		*/
	case 'M':	f *= 256ull;	/* 2 Mega		*/
	case 'C':	f *= 4ull;	/* 4096 CD-Size		*/
	case 'K':	f *= 2ull;	/* 1 Kilo		*/
	case 'S':	f *= 512ull;	/* 512 Sector size	*/
	case 'B':	break;
	}
      if (is_signed)
	{
	  o	= (long long)ull*f;
	  /* For some unknown reason, signed/unsigned=unsigned	*/
	  if ((long long)o/(long long)f!=(long long)ull)
	    {
	      fprintf(stderr, "getopt: option %.*s overflow by suffix: %s %lld (%lldx%llu)\n", p->optlen, p->opt, end-1, (long long)o, (long long)ull, f);
	      if (!p->IGNERR_var)
		return -3;
	    }
	}
      else
	{
	  o	= ull*f;
	  if (o/f!=ull)
	    {
	      fprintf(stderr, "getopt: option %.*s overflow by suffix: %s (%llux%llu)\n", p->optlen, p->opt, end-1, ull, f);
	      if (!p->IGNERR_var)
		return -3;
	    }
	}
      ull	= o;
    }
  if (end && *end && p->TIMESPEC_var)
    {
      unsigned long long	o, f;

      f	= 1;
      switch (*end++)
	{
	default:
	  fprintf(stderr, "getopt: option %.*s unknown timespec: %s\n", p->optlen, p->opt, end-1);
	  if (!p->IGNERR_var)
	    return -3;
	  break;

	  /* estimates rounded up	*/
	case 'C':	f *= 36525ull; if (0)	/* Century	*/
	case 'D':	f *= 3653ull; if (0)	/* Decade=10y	*/
	case 'Y':	f *= 366ull; if (0)	/* Year		*/
	case 'S':	f *= 92ull; if (0)	/* Season	*/
	case 'M':	f *= 31ull; if (0)	/* Month	*/

	  /* exact	*/
	case 'w':	f *= 7ull;		/* Week	*/
	case 'd':	f *= 24ull;		/* Day	*/
	case 'h':	f *= 60ull;		/* Hour	*/
	case 'm':	f *= 60ull;		/* Minute	*/
	case 's':	break;			/* Seconds	*/
	}
      if (is_signed)
	{
	  o	= (long long)ull*f;
	  /* For some unknown reason, signed/unsigned=unsigned	*/
	  if ((long long)o/(long long)f!=(long long)ull)
	    {
	      fprintf(stderr, "getopt: option %.*s overflow by timespec: %s (%lldx%llu)\n", p->optlen, p->opt, end-1, (long long)ull, f);
	      if (!p->IGNERR_var)
		return -3;
	    }
	}
      else
	{
	  o	= ull*f;
	  if (o/f!=ull)
	    {
	      fprintf(stderr, "getopt: option %.*s overflow by timespec: %s (%llux%llu)\n", p->optlen, p->opt, end-1, ull, f);
	      if (!p->IGNERR_var)
		return -3;
	    }
	}
      ull	= o;
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
      return -2;

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
  if (!is_signed)
    fprintf(stderr, "getopt: value %llu out of bounds for option %.*s: %s\n", ull, p->optlen, p->opt, arg);
  else
    fprintf(stderr, "getopt: value %lld out of bounds for option %.*s: %s\n", (long long)ull, p->optlen, p->opt, arg);

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

/* Process some option, set the args.
 * invert=0 normally.  -1 if we should see for a +, 1 if it is a +opt LLOPT
 * returns:
 * 0	if it is a direct option (so more options can follow)
 * 1	if then option was used completely
 * 2	if the option and the next argument was used completely
 * -1	help requested
 * -2	internal error
 * -3	option error (unless IGNERR, else 1 or 2)
 */
static int
tino_getopt_var_set_arg(struct tino_getopt_impl *p, const char *arg, const char *next, int invert)
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
      invert	= 0;
    }
  else if ((p->LLOPT_var || p->LOPT_var || p->DD_var) && *arg)
    {
      /* Long options have --"long"=arg or --"long."arg or --"long arg"
       * (--"long" "arg" has !*arg).
       * --long above can be +long on PLUS LLOPT or -long on LOPT
       *
       * In case of PLUS_var on _FLAG we see invert==-1, in this case
       * (and only in this) the next character can be '+' to invert.
       */
      if (isalnum(p->opt[p->optlen-1]))	/* option ends on AlNum	*/
	{
	  /* skip next character, which must be a separator	*/
	  if (*arg++!='+')		/* keep invert on --long+ or +long+	*/
	    invert	= 0;
	}
      else if (invert<0 && *arg=='+')	/* --_long_+ or --long.+ or similar	*/
	arg++;
      else
	invert	= 0;
    }
  else
    invert	= 0;
  n	= tino_getopt_var_set_arg_imp(p, arg, n, invert);
  if (n<0 || !p->FN_var)
    return n;

  s	= p->FN_var(p->varptr, arg, p->opt, p->USER_var);
  if (!s)
    return n;

  if (*s)
    fprintf(stderr, "getopt: rejected option %.*s: %s\n", p->optlen, p->opt, s);
  return -2;
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
tino_getopt_init(TINO_VA_LIST list, struct tino_getopt_impl *q, int max)
{
  int		opts;
  const char	*global;

  /* Parse the first argument (global):
   * Version
   * Compilation date
   * Global options
   * Usage string
   */
  global	= TINO_VA_STR(list);
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
      if (q[opts].NODEFAULT_var)
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

      /* DEFAULT_PTR takes precedence
       */
      if (q[opts].DEFAULT_PTR_var)
	tino_getopt_var_set_ptr(q+opts, q[opts].DEFAULT_PTR_var);
      /* environment overrides any other setting
       */
      if (q[opts].DEFAULT_ENV_var && getenv(q[opts].DEFAULT_ENV_var))
	tino_getopt_var_set_arg_imp(q+opts, getenv(q[opts].DEFAULT_ENV_var), 0, 0);

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

  if (q[0].TAR_var || q[0].POSIX_var)
    fprintf(stderr, "getopt: tar/posix not yet implemented, continuing anyway\n");

  if (q[0].TAR_var && argc>1 && argv[1][0]!='-')
    {
      /* First option is TAR options
       * Hunt through all the TAR options and process them ..
       * (or do this below?)
       * XXX TODO XXX support parsing of TAR like options
       */
      TINO_XXX;
    }

  /* Well, what follows is too long actually.
   * However it's difficult to break it up.
   */
  for (pos=0; ++pos<argc; )
    {
      const char	*ptr;

/* return 0 if --opt or --opt=value or --opt+ (for PLUSopts)
 * If option does not end on AlNum, the separator can be missing, so it is detected anyway.
 */
#define	TINO_GETOPT_CMPOPT_LONG(I,INV)	(INV)>=0 || ptr[q[I].optlen]!='+'
#define	TINO_GETOPT_CMPOPT_SHORT(I)	q[I].optlen>1
#define	TINO_GETOPT_CMPOPT(I,TEST)	(!q[I].optlen ||			\
					 strncmp(ptr, q[I].opt, q[I].optlen) ||	\
					 (ptr[q[I].optlen]			\
					  && ptr[q[I].optlen]!='='		\
					  && (TEST)				\
					  && isalnum(q[I].opt[q[I].optlen-1])	\
					  )					\
					 )

#define	TINO_GETOPT_PROCESSLONGOPT(I,COND,INV)						\
	      if (!(COND) || TINO_GETOPT_CMPOPT(I,TINO_GETOPT_CMPOPT_LONG(I,INV)))	\
		continue;								\
	      ptr	+= q[I].optlen;							\
	      I		= tino_getopt_var_set_arg(q+I, ptr, argv[pos+1], INV);		\
	      if (!I && *ptr)								\
		{									\
		  fprintf(stderr, "getopt: flag %s must not have args\n", argv[pos]);	\
		  return -2;								\
		}
	      /* i<0	help option or error
	       * i==0	last thing was flag (so iterate to next character)
	       * i==1	last thing was argument
	       * i==2	one addional argv was eaten away
	       */

      ptr	= argv[pos];
      if (*ptr=='+' && ptr[1] && q[0].PLUS_var)	/* + on it's own always is an ARG	*/
	{
	  for (i=opts;;)
	    {
	      if (--i<1)
		{
		  fprintf(stderr, "getopt: unknown option +%s\n", ptr);
		  return -2;
		}
	      TINO_GETOPT_PROCESSLONGOPT(i,q[i].LLOPT_var, 1);	/* +opt needs LLOPTs	*/
	      break;
	    }
	}
      else if (*ptr=='-' && *++ptr)		/* - on it's own always is an ARG	*/
	{
	  /* we always recognize - or -- as start of options
	   * XXX TODO XXX add killswitch for DD/TAR-only type programs?
	   */
	  if (*ptr=='-')	/* we saw --	*/
	    {
	      /* end of options: --
	       * Make it unknown in situations where we do not process '-' at all,
	       * so it becomes an ARG
	       */
	      if (!*++ptr && (q[0].POSIX_var || q[0].PLUS_var || q[0].LOPT_var || q[0].LLOPT_var || !(q[0].DD_var || q[0].TAR_var)))
		{
		  pos++;
		  break;	/* same as: return pos	*/
		}
	      /* --long option
	       */
	      for (i=opts;;)
		{
		  if (--i<1)
		    {
		      fprintf(stderr, "getopt: unknown option --%s\n", ptr);
		      return -2;
		    }
		  TINO_GETOPT_PROCESSLONGOPT(i,q[i].LLOPT_var, (q[i].type==TINO_GETOPT_TYPE_FLAG && q[i].PLUS_var ? -1 : 0));
		  break;
		}
	      /* The --option has been processed successfully if i>=0
	       */
	    }
	  else
	    {
	      /* short option (preceeding is -) or LOPT
	       *
	       * Notes:
	       * - *ptr is pointing to some character.
	       * - -something always is considered to be an option.
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
		    } while (((q[i].LLOPT_var || q[i].DD_var) && !q[i].LOPT_var) ||	/* LLOPT processed above, DD never start on -	*/
			     TINO_GETOPT_CMPOPT(i, TINO_GETOPT_CMPOPT_SHORT(i)));
		  ptr	+= q[i].optlen;
		  i	= tino_getopt_var_set_arg(q+i, ptr, argv[pos+1], 0);
		  if (i)
		    break;
		} while (*ptr);
	      /* All options have been processed successfully if i>=0
	       */
	    }
	  /* short or long option
	   * i<0	help option or error
	   * i==0	last thing was flag (so iterate to next character)
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
	      TINO_GETOPT_PROCESSLONGOPT(i,q[i].DD_var, 0);
	      /* i<0	help option or error
	       * i==0	last thing was flag (so iterate to next character)
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
       * XXX TODO XXX support TAR like options
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
       * XXX TODO XXX support POSIX mode where options can follow the arguments
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

      if (q[i].type<0)
	continue;

      fputc('\t', stderr);
      tino_getopt_print_option_name(q+i, 1, "\n");
      s	= "\t\t(";
      for (j=0; ++j<opts; )
	if (j!=i && q[j].varptr==q[i].varptr && q[i].type>TINO_GETOPT_TYPE_HELP && q[j].type>TINO_GETOPT_TYPE_HELP)
	  {
	    fprintf(stderr, "%ssee ", s);
	    tino_getopt_print_option_name(q+j, 0, NULL);
	    s	= ", ";
	    break;
	  }
      if (q[i].DEFAULT_ENV_var)
	{
	  fprintf(stderr, "%senv '%s'", s, q[i].DEFAULT_ENV_var);
	  s	= " ";
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
	  fprintf(stderr, "%s%s ", s, (q[i].type==TINO_GETOPT_TYPE_STRINGFLAG || !(q[i].MAX_var || q[i].MAX_PTR_var) ? "set to" : "from"));
	  s	= tino_getopt_var_to_str(&q[i].min, q[i].type, auxbuf);
	  fprintf(stderr, (s==auxbuf ? "%s" : "'%s'"), s);
	  s	= " ";
	}
      if (q[i].MIN_PTR_var)
	{
	  fprintf(stderr, "%s%s* ", s, (q[i].type==TINO_GETOPT_TYPE_STRINGFLAG || !(q[i].MAX_var || q[i].MAX_PTR_var) ? "set to" : "from"));
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
      if (*s!='\t')
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
		 TINO_VA_LIST list,
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
  opts	= tino_getopt_init(list, q+1, (sizeof q/sizeof *q)-1);

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
  ret	= tino_getopt_hook(argc, argv, min, max, &list, NULL, NULL);
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
