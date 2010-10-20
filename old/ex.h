/* $Header$
 *
 * Error and warning output.
 *
 * I AM NOT HAPPY WITH THIS!
 *
 * Copyright (C)2004-2008 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 *
 * $Log$
 * Revision 1.25  2010-10-20 23:12:04  tino
 * failing execv of proc-fork returns 127 (like bash) instead of -1
 *
 * Revision 1.24  2008-09-20 18:04:05  tino
 * file locks
 *
 * Revision 1.23  2008-09-02 16:24:04  tino
 * Prefix now better supported
 *
 * Revision 1.22  2008-05-21 17:57:58  tino
 * Missing output ": " added
 *
 * Revision 1.21  2008-05-19 09:10:43  tino
 * tino_verror_ext
 *
 * Revision 1.20  2007-12-10 02:42:29  tino
 * See diff
 *
 * Revision 1.19  2007-10-04 12:55:48  tino
 * bugfix in tino_file_mkdirs_forfile and less compile clutter
 *
 * Revision 1.18  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.17  2007/04/16 19:52:21  tino
 * See ChangeLog
 *
 * Revision 1.16  2006/10/21 01:43:05  tino
 * va_list changes
 *
 * Revision 1.15  2006/10/04 01:57:12  tino
 * tino_va_* functions for better compatibility
 *
 * Revision 1.14  2006/08/14 04:21:13  tino
 * Changes for the new added curl.h and data.h
 *
 * Revision 1.13  2006/07/22 23:47:43  tino
 * see ChangeLog (changes for mvatom)
 *
 * Revision 1.12  2005/12/08 01:41:52  tino
 * TINO_VEXIT changed
 *
 * Revision 1.11  2005/12/05 02:11:12  tino
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
 */

#ifndef tino_INC_ex_h
#define tino_INC_ex_h

#include "arg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

/* This is overwritten by exception.h
 */
#ifndef TINO_EXIT
#define	TINO_EXIT(X)		TINO_ERROR_PREFIX(exit,X)
#define	TINO_VEXIT(LIST)	TINO_ERROR_PREFIX(vexit,(LIST))
#define	TINO_VPEXIT(P,LIST)	TINO_ERROR_PREFIX(vpexit,(P,LIST))
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

static void
tino_verror_vext(TINO_VA_LIST prefix, TINO_VA_LIST list, int err)
{
  fflush(stdout);
  if (prefix->str)
    {
      tino_vfprintf(stderr, prefix);
      fprintf(stderr, ": ");
    }
  tino_vfprintf(stderr, list);
  if (err)
    fprintf(stderr, ": %s\n", strerror(err));
  else
    fputc('\n', stderr);
  fflush(stderr);
}

static void
tino_verror_ext(TINO_VA_LIST list, int err, const char *prefix, ...)
{
  tino_va_list	list2;

  tino_va_start(list2, prefix);
  tino_verror_vext(&list2, list, err);
  tino_va_end(list2);
}

static void
tino_verror_std(const char *prefix, TINO_VA_LIST list, int err)
{
  if (prefix)
    tino_verror_ext(list, err, "%s", prefix);
  else
    tino_verror_ext(list, err, NULL);
}

static void (*tino_verror_fn)(const char *, TINO_VA_LIST, int);

static void
tino_verror(const char *prefix, TINO_VA_LIST list, int err)
{
  (tino_verror_fn ? tino_verror_fn : tino_verror_std)(prefix, list, err);
}

static void
tino_error(const char *prefix, const char *s, ...)
{
  tino_va_list	list;
  int		err;

  err	= errno;
  tino_va_start(list, s);
  tino_verror(prefix, &list, err);
  tino_va_end(list);
}

#ifdef TINO_NEED_OLD_ERR_FN
/* This now changed meaning and was switched to the new global error
 * handler, see err.h
 */
static void
tino_verr(TINO_VA_LIST list)
{
  int		err;

  err	= errno;
  tino_verror("error", list, err);
}

static void
tino_err(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_verr(&list);
  tino_va_end(list);
}
#endif

static void
tino_vwarn(TINO_VA_LIST list)
{
  tino_verror("warning", list, 0);
}

static void
tino_warn(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vwarn(&list);
  tino_va_end(list);
}

static void
tino_vpexit_n(int n, const char *prefix, TINO_VA_LIST list)
{
  tino_verror(prefix, list, errno);
  if (n==0)
    n = -1;
  TINO_ABORT(n);
}

static void
tino_vpexit(const char *prefix, TINO_VA_LIST list)
{
  tino_vpexit(prefix, list);
}

static void
tino_vexit_n(int n, TINO_VA_LIST list)
{
  tino_vpexit_n(n, NULL, list);
}

static void
tino_vexit(TINO_VA_LIST list)
{
  tino_vexit_n(-1, list);
}

static void
tino_exit(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vexit(&list);
  /* never reached	*/
}

static void
tino_exit_n(int n, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vexit_n(n, &list);
  /* never reached	*/
}

#endif
