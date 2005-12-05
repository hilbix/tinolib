/* $Header$
 *
 * Error and warning output.
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
 * Revision 1.11  2005-12-05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.10  2005/03/04 00:42:36  tino
 * tino_verror_fn hook added
 *
 * Revision 1.9  2005/01/26 10:51:57  tino
 * Changes for updated exception.h
 *
 * Revision 1.8  2005/01/25 22:14:51  tino
 * exception.h now passes include test (but is not usable).  See ChangeLog
 *
 * Revision 1.7  2004/04/13 00:29:12  tino
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

/* This is overwritten by exception.h
 */
#ifndef TINO_EXIT
#define	TINO_EXIT(X)		TINO_ERROR_PREFIX(exit,X)
#define	TINO_VEXIT(X)		TINO_ERROR_PREFIX(vexit,X)
#endif

#ifndef TINO_ERROR_PREFIX
/* Ugly hack
 */
#define TINO_ERROR_PREFIX(X,Y)	do { tino_error_prefix(__FILE__,__LINE__,__FUNCTION__); tino_##X Y; } while (0)
static void
tino_error_prefix(const char *file, int line, const char *fn)
{
  fprintf(stderr, "%s:%d:%s: ", file, line, fn);
}
#endif

#ifndef TINO_ABORT
#include <unistd.h>
#define TINO_ABORT(X)	exit((X)); _exit((X)); abort(); for(;;)
#endif

#if 0
static int tino_global_error_count;
#endif

static void (*tino_verror_fn)(const char *, const char *, va_list, int);

static void
tino_verror(const char *prefix, const char *s, va_list list, int err)
{
  if (tino_verror_fn)
    {
      tino_verror_fn(prefix, s, list, err);
      return;
    }
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
  TINO_ABORT(-1);
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
