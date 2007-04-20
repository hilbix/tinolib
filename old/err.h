/* $Header$
 *
 * Please note the usage pattern for TINO_ERR at the end of this file.
 *
 * IF YOU INCLUDE THIS FILE AND PREVIOUSLY USED ex.h THEN BE SURE TO
 * ADD A PROPER ERROR TAG TO ALL ERROR TEXTS IN CALLS TO tino_err()
 * and tino_verr()!
 *
 * NOT READY YET *
 * UNIT TEST FAILS *
 *
 * One central generic error, messaging and hook handler
 *
 * Copyright (C)2007 by Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.3  2007-04-20 20:49:48  tino
 * TYPE_ERR (E) added
 *
 * Revision 1.2  2007/04/20 20:24:44  tino
 * fixed
 *
 * Revision 1.1  2007/04/16 19:52:21  tino
 * See ChangeLog
 *
 */

#ifndef tino_INC_err_h
#define tino_INC_err_h

#include "ex.h"

/** This is (the start of) a generic error handling
 *
 * Errors tags are short strings which can refernce the complete error
 * string.  This string than can be translated into other languages
 * (by chance) or explained in short or long form, whatever.  Also you
 * have a good chance to Google this error easily.
 *
 * Error tags look like ETLF001A and generally have the form ABBBnnnC
 * where A is a single letter, BBB is a prefix with subsecelction, n
 * is a number which shall have 3 digits minimum, where 000 is not
 * allowed, and terminated with a single letter C telling how to handle
 * this error.
 *
 * Despite the fact that this is called "err" this can be used from
 * fatals to informational messages, too.
 *
 * You might be nerved by the fact that you always have to write the
 * full error tag.  However this is important for search engines to be
 * able to detect the error information, this way you can search for
 * it on a global basis.  So please stick at this simeple fact: Always
 * give it in full.
 */

/** First letter A in ABBBnnnC.
 *
 * F	fatals
 * E	errors
 * W	warnings
 * N	notes
 * I	info
 *
 * Message hook types:
 *
 * d	debug (output)
 * s	signal (EINTR from syscall)
 * h	hooks (to hook some functionality)
 * m	messages (internal message processing)
 *
 * Notes on default behavior:
 *
 * F: Error handler must return TINO_ERR_RET_PRAY to make fatals
 * nonfatal.  This is make the program not terminating.  However,
 * behavior of programs are not defined afterwards!  So you shall only
 * use this on errors from applications.  For example, if you override
 * an OOM situation, the library might segfault, as it does not
 * excpect NULL pointers later on.
 *
 * E: For TINO_ERR_TYPE_FATAL you must give TINO_ERR_RET_OVERRIDE to
 * allow the program to continue.  However the routine is able to
 * continue.  But note that it might give some unusal return values,
 * such that the program might segfault as it does not expect NULL
 * return values.
 *
 * E: For TINO_ERR_TYPE_CRIT you must give TINO_ERR_RET_DOWNGRADE to
 * allow the program to continue.  However this usually is a normal
 * situation and normal programs shall be able to cope with this
 * situation.  Critical situations are something like "well, the
 * hacker has broken my security and is eavesdropping.  I think it's
 * not clever to continue."
 *
 * E: For TINO_ERR_TYPE_IGNORE default is to ignore this error, this
 * is, even without handler the error routine returns and does not
 * abort the program.
 *
 * W: Normally there is no type field for this.  The default is not to
 * terminate the program, except for TINO_ERR_TYPE_CRIT and
 * TINO_ERR_TYPE_FATAL.  However it's enough to give
 * TINO_ERR_RET_DOWNGRADE from the error handler to continue the
 * program.  You might think that a "fatal warnings" is something
 * funny, but it is something like "the security certificate is void".
 * This not really is an error but a very important situation.
 *
 * N: Normally there is no type field for this.  The default for
 * TINO_ERR_TYPE_FATAL notes is to terminate the program.  However
 * it's enough to give TINO_ERR_RET_IGNORE to continue.
 *
 * I: The type is ignored completely, you shall leave it away.
 */

#define TINO_ERR_ANY	"?"	/* any uppercase tags	*/
#define TINO_ERR_FATAL	"F"
#define TINO_ERR_ERROR	"E"
#define TINO_ERR_WARN	"W"
#define	TINO_ERR_NOTE	"N"
#define	TINO_ERR_INFO	"I"

/** First letter A in ABBBnnnC continued for lowercase letters.
 *
 * Lowercase letters are no real errors but internal messages to 'hook
 * in' you own extensions.  They are routed through the error handler
 * functions until one returns non-zero.  Default action is to just
 * ignore these and return 0.
 *
 * You must register handlers explicitely for this letters.  There is
 * no "any" type.
 *
 * The first handler which returns non-zero aborts the hook chain and
 * the value of the handler is returned.  If all return 0 (or none is
 * present) just 0 is returned.
 *
 * Note on types:
 *
 * s: Many functions know about TINO_ERR_RET_ABORT and
 * TINO_ERR_RET_RETRY (dito for TINO_ERR_RET_IGNORE), where the first
 * means to return from the subroutine and the latter is to restart
 * the syscall (if possible).
 *
 * d: Shall really be used for debug types only.  This mainly is
 * thought for debugging which users can enable, not for the debug.h
 * type.  But I might consider to move debugging this way.
 *
 * h: These are provided for future extensions, but not as a general
 * internal signaling mechanism within a library.  Use type m instead.
 *
 * m: Internal messaging and message queuing.  Usually the message is
 * only received by the handler and not processed immediately.  For
 * messages to immediately process, use type i instead.
 *
 * i: Immediate messages providing some interface.  Note that the
 * return is somewhat limited, however there can be argument passing.
 */
#define	TINO_ERR_DEBUG	"d"	/* Debugging output	*/
#define	TINO_ERR_SIGNAL	"s"	/* EINTR received	*/
#define	TINO_ERR_HOOK	"h"	/* Genereal hooks	*/
#define	TINO_ERR_MSG	"m"	/* Internal messaging	*/
#define	TINO_ERR_IF	"i"	/* Interface messages	*/


/** Last letter C in ABBBnnnC
 *
 * Normally you will see type A (first letter in alphabet) as programs
 * usually cannot detect if operations are impossible.  Example:
 * Creating a file on a nonexisting path can be retried when path is
 * created, so it is type A.  Something like an impossible IP number
 * (257.1.1.1) is type B, however all type B may choose to use type A,
 * as often libraries are not that clever.
 *
 * Notes on types:
 *
 * Missing: The missing type (and all unknown types) are handled like
 * type A.  Missing types are no errors itself!
 *
 * R: This are the "Abort, Retry, Ignore" types of errors, where the
 * error handler can choose.  Default is 'abort'!
 *
 * I: For this error types default is "report but retry"!  Typical
 * things are "accept errors" which happens often.
 */
#define TINO_ERR_TYPE_AGAIN	"A"	/* again, operation temporarily failed	*/
#define TINO_ERR_TYPE_BREAK	"B"	/* break, operation not possible	*/
#define	TINO_ERR_TYPE_CRIT	"C"	/* critical, default terminate program	*/
#define TINO_ERR_TYPE_ERR	"E"	/* error, program terminates	*/
#define TINO_ERR_TYPE_FATAL	"F"	/* fatal, will break things	*/
#define TINO_ERR_TYPE_IGNORE	"I"	/* ignore, default is to ignore	*/
#define TINO_ERR_TYPE_RETRY	"R"	/* retryable, retry if error handler wants	*/
#define TINO_ERR_TYPE_TRANSIENT	"T"	/* transient, operation partly failed	*/

/* Other generic (reserved) letters, all are "promoted" to type A
 *
 * Note that all letters which are not listed here might get another
 * meaning sometimes in future!
 *
 * J to Q can be (ab)used as priority markers 0 to 7, where this is
 * application generic.
 */
#define	TINO_ERR_TYPE_GENERAL	"G"	/* general	*/
#define	TINO_ERR_TYPE_HIGH	"H"	/* high	*/
#define	TINO_ERR_TYPE_JAY	"J"	/* 0: Agent J (too much MiB?)	*/
#define	TINO_ERR_TYPE_KAY	"K"	/* 1: Agent K	*/
#define	TINO_ERR_TYPE_LOW	"L"	/* 2: low	*/
#define	TINO_ERR_TYPE_MESSAGE	"M"	/* 3: message	*/
#define	TINO_ERR_TYPE_NOTE	"N"	/* 4: note	*/
#define	TINO_ERR_TYPE_OPTION	"O"	/* 5: option	*/
#define	TINO_ERR_TYPE_PROGRAM	"P"	/* 6: program	*/
#define	TINO_ERR_TYPE_QUERY	"Q"	/* 7: query	*/

/** The error handler function is designed generic for future
 * portability.  This is, the structure might get some new meanings,
 * but the interface basically stays the same.
 *
 * The structure continas all important information for the error
 * handler, such that it is able to do all the things needed.
 *
 * The error handler must return a value.  If it returns -1 the
 * program always terminates with a fatal (abort()), except for
 * TINO_ERR_DEBUG types.  You can return other values, these are
 * promoted to the next lower result if needed.
 */
#define	TINO_ERR_RET_ABORT	-1
#define	TINO_ERR_RET		0	/* do default action	*/
#define	TINO_ERR_RET_RETRY	1
#define	TINO_ERR_RET_IGNORE	2
#define	TINO_ERR_RET_DOWNGRADE	24	/* TINO_ERR_RET_IGNORE for type C	*/
#define	TINO_ERR_RET_OVERRIDE	42	/* TINO_ERR_RET_IGNORE for type F	*/
#define	TINO_ERR_RET_PRAY	666	/* TINO_ERR_RET_IGNORE for TINO_ERR_FATAL	*/

#if 0
struct tino_err_info
  {
    /* TBD	*/
  };
typedef int tino_err_handler_fn(struct tino_err_info *);

/** Register error translation strings with a prefix
 *
 * Following reserved prefixes exist, this is, you must not use
 * prefixes starting with these if they are not reserved for you.  If
 * you have an OID under 1.3.6.1.4.1 (see also
 * http://www.alvestrand.no/objectid/1.3.6.1.4.1.html and
 * http://oid.elibel.tm.fr/1.3.6.1.4.1 and RFC 2252) you can use your
 * number with 1 prefixed, too.  Thus Skytec AG (8505) would use 18505
 * and IBM would use 12.  It is most important to end this number with
 * one or more letters to separate it from the trailing number.
 *
 * To get a prefix reservation, please contact me:
 * <webmaster@scylla-charybdis.com>
 *
 * T	Reserved for me (anything starting with T)
 * TLx	TinoLib where x is the library area
 * TTx	TinoTool where x is the tool
 * _	local, unspec, never reserved
 *
 * Note that you can set prefix to NULL to get if from dshort/dlong.
 * However if you set it, the values in dshort/dlong are checked if
 * they contain the correct prefix (this is BBB in ABBBnnnC).  Don't
 * mix this prefix with the prefix of tino_err_handler_set.
 *
 * 'dshort' and 'dlong' are lists of error strings, where 'dshort' has
 * the form
 *
 * "ABBBnnnC short description of first error tag\0"
 * "ABBBnnnC short description of second error tag\0"
 *
 * and so on, while 'dlong' has the form
 *
 * "ABBBnnnC short description of first error tag\n"
 * "first line of long description of first error tag\n"
 * "second line of long description fo first error tag\n"
 * more lines
 * "\0"
 " "ABBBnnnC short description of second error tag\n"
 * "first line of long description of second error tag\n"
 * "second line of long description fo first error tag\n"
 * more lines
 * "\0"
 *
 * and so on.  If both, 'dshort' and 'dlong' are given (are not NULL),
 * the 'short description' in 'dlong' can be left away, so that the
 * line only contans the error tag "ABBBnnnC" and the NL, of course.
 *
 * Note that you cannot unregister anything!  You only can add things.
 *
 * If more than one explanation for one error TAG is added, it is
 * undefined which one wins.  However routines can iterate through all
 * possibilities.
 *
 * Note about variable parts of the description:

 * Replacements are done for curly bracket sequences '{..}'.  There
 * are some specials: '{{' becomes '{', and '{whitespace' is output
 * unchanged.  Both do not open a sequence.  Within a sequence you can
 * use '{}' to enter a '}'.  A single number '{n}' starting from 1
 * refers to the Nth parameter.  '{0}' refers to the error tag.  The
 * data type is taken from the parameter specification to tino_err().
 * Formatting rules see 
 */
static void
tino_err_register(const char *prefix, const char *dshort, const char *dlong)
{
  000;
}

/** Set the error handler function
 *
 * You can select the type (TINO_ERR_ANY for any) and a prefix to
 * react on (NULL for any).  Also you can give a USER pointer.
 *
 * This returns the old error handler which was active.  If you set
 * the error handler to NULL this selects the default error action to
 * take place.
 *
 * Note that "prefix" must start with the error to accept, use
 * TINO_ERR_ANY if you want eny.
 */
static tino_err_handler_fn *
tino_err_handler_set(const char *prefix, tino_err_handler_fn *fn, void *user)
{
  000;
}

/** Delete an error handler function.  To unset all error handlers do:
 *
 * while (tino_err_handler_delete(NULL, NULL, NULL)>0);
 *
 * If you want to select a specific type of error handler, then you
 * can preset this on the parameters variables.  Set the parameter to
 * TINO_ERR_ANY or NULL to get any matching handler in this respect.
 * The variables will be modified to give back the exact values.
 *
 * The routine returns -1 for error, 0 for nothing found and 1 for
 * something found.
 */
static int
tino_err_handler_delete(const char *prefix, tino_err_handler_fn **fn, void **user)
{
  000;
}

/** Hunt for 'err'or tag and fill in 'inf' structure.
 *
 * If inf==NULL it just does nothing.  If err==NULL then the error tag
 * is taken from inf (as inf is considered initialized) and inf is set
 * to the first matching entry (return 1).  If err==TINO_ERR_FILL_NEXT
 * then the next description is searched (if this is the first search,
 * then this returns the first match).
 *
 * Return value is -1 for "not found", 0 for "nothing else found" (inf
 * is initialized to first possible value), 1 for "found".  Following
 * outputs all possible values (for error handlers).  Replace NULL
 * with the error you search if you are outside error handlers:
 *
 * tino_err_hunt(inf, NULL);	// preset to first even if not at first
 * do
 *   {
 *     tino_err_expand(inf->decr_long, inf, NULL);
 *   } while (tino_err_hunt(inf, TINO_ERR_FILL_NEXT)>0);
 *
 * Special: If you call it with err=="" then you get a list of all
 * known errors with all known explanations.  This is true for all
 * other partial prefixes, too, and this feature is intentional:
 *
 * tino_err_hunt(inf, "");	// output all known errors
 * do
 *   {
 *     tino_err_expand(inf->decr_long, inf, NULL);
 *   } while (tino_err_hunt(inf, TINO_ERR_FILL_NEXT)>0);
 */
#define	TINO_ERR_FILL_NEXT	((const char *)1)
static int
tino_err_hunt(struct tino_err_info *inf, const char *err)
{
  000;
}

/** Expand the description text of an error.
 *
 * If io==NULL output goes to stderr.  Else the 'io' routine is called
 * for each piece of data assembled.  For 
 */
static void
tino_err_expand(const char *txt, struct tino_err_info *inf, struct tino_err_io *io)
{
  000;
}
#endif

/** The Generic error function for *all* types of errors and
 * synchronous callbacks.
 *
 * Most times you can ignore the return value.  The return value is 0
 * (default) or 1 (handler returned TINO_ERR_RET_RETRY).  On
 * TINO_ERR_DEBUG this returns the value the handler call returned.
 *
 * The id_params_short parameter is a string which starts with the
 * error tag (ABBBnnnC from above), a blank and a string which
 * contains all parameters like in a printf situation.  The string,
 * however, may be overwritten by registered error strings.  Only if
 * none found it is output this way.
 *
 * Important: If you change the sequence of the parameter list, be
 * sure to change the references in the descriptions, too.  If you
 * change the data type, you do not need this.
 *
 * This routine currently is not optimized to quickly map error tags
 * to the handlers.  Luckily in most situations you will only have few
 * handlers.
 *
 * To speed up things, it does not hunt for the error tag, you must do
 * this in your error handler if you need the translation.  The
 * function is tino_err_hunt(inf, NULL).
 *
 * Notes:
 *
 * The opt_tag_params_short can be 0x2 or 0x3 for prefix usage (see
 * macro TINO_ERR below).  The args are then pulled from the argument
 * list accordingly.
 *
 * opt_tag_params_short can start with "%s", then the error tag is
 * taken as the first argument.  Note that it cannot be constructed,
 * it must always be either a complete tag or "%s" with the complete
 * tag on the argument list.
 */
static void
tino_verr(const char *opt_tag_params_short, TINO_VA_LIST list)
{
  /* This is only for old compatibility, it will be removed in future!
   */
  if (opt_tag_params_short==(void *)2 || opt_tag_params_short==(void *)3)
    {
      const char	*file, *fn=0;
      int		line;

      file	= TINO_VA_ARG(list, const char *);
      line	= TINO_VA_ARG(list, int);
      if (opt_tag_params_short==(void *)3)
	fn	= TINO_VA_ARG(list, const char *);
      tino_error_prefix(file, line, fn);
      opt_tag_params_short	= TINO_VA_ARG(list, const char *);
    }
  tino_verror("error", opt_tag_params_short, list, errno);
}

/** See tino_verr().  If you previously used ex.h, be sure to add the
 * proper error tag in front of your error message.
 *
 * If you give the error description in the error text, please start
 * to use the new format: "TAG parameters text", where parameters are
 * %s%p%d and so on (without formatting).  The blank also is important
 * and be sure that any % sign in the text is doubled.  If this is not
 * true, the deprecated form is assumed and a warning is issued.
 */
static int
tino_err(const char *opt_tag_params_short, ...)
{
  tino_va_list	list;

  tino_va_start(list, opt_tag_params_short);
  tino_verr(opt_tag_params_short, &list);
  tino_va_end(list);
  return TINO_ERR_RET;
}

/* Use this like in
 *
 * tino_err(TINO_ERR(ETL100A,%p%d%s), ptr, integer, string);
 *
 * This will send additional parameters to the error routine
 */
#ifdef __FUNCTION__
#define	TINO_ERR(TAG,PARAMS)	(void *)3, __FILE__, __LINE__, __FUNCTION__, #TAG " " #PARAMS
#else
#define	TINO_ERR(TAG,PARAMS)	(void *)2, __FILE__, __LINE__, #TAG " " #PARAMS
#endif

#endif
