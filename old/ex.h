/* $Header$
 *
 * $Log$
 * Revision 1.7  2004-04-13 00:29:12  tino
 * A lot of changes to do compile.  Far from ready yet.
 *
 * Revision 1.6  2004/04/07 02:22:48  tino
 * Prototype for storing data in gff_lib done (untested)
 *
 * Revision 1.5  2004/03/28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.4  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.3  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
 *
 * Revision 1.2  2004/03/23 21:35:08  tino
 * error, verror, vex
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

#ifndef tino_INC_ex_h
#define tino_INC_ex_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

static int tino_global_error_count;

static void
tino_verror(const char *prefix, const char *s, va_list list, int err)
{
  tino_global_error_count++;
  if (prefix)
    fprintf(stderr, "%s: ", prefix);
  vfprintf(stderr, s, list);
  if (err)
    fprintf(stderr, ": %s\n", strerror(err));
  else
    fputc('\n', stderr);
}

static void
tino_error(const char *prefix, const char *s, ...)
{
  va_list	list;
  int		err;

  err	= errno;
  va_start(list, s);
  tino_verror(prefix, s, list, err);
  va_end(list);
}

static void
tino_err(const char *s, ...)
{
  va_list	list;
  int		err;

  err	= errno;
  va_start(list, s);
  tino_verror("error", s, list, err);
  va_end(list);
}

static void
tino_vwarn(const char *s, va_list list)
{
  tino_verror("warning", s, list, 0);
}

static void
tino_warn(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_vwarn(s, list);
  va_end(list);
}

static void
tino_vexit(const char *s, va_list list)
{
  tino_verror(NULL, s, list, errno);
  exit(-1);
  abort();
  for(;;);
}

static void
tino_exit(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_vexit(s, list);
  /* never reached	*/
}

#endif
