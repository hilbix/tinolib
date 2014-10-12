/* Error and warning output.
 *
 * I AM NOT HAPPY WITH THIS!
 *
 * Copyright (C)2004-2014 Valentin Hilbig, webmaster@scylla-charybdis.com
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
#ifndef TINO_EXIT_DEFAULT_CODE
#define	TINO_EXIT_DEFAULT_CODE	-1
#endif
static int tino_exit_default_code = (TINO_EXIT_DEFAULT_CODE);

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
    n = tino_exit_default_code;
  TINO_ABORT(n);
}

static void
tino_vpexit(const char *prefix, TINO_VA_LIST list)
{
  tino_vpexit_n(0, prefix, list);
}

static void
tino_vexit_n(int n, TINO_VA_LIST list)
{
  tino_vpexit_n(n, NULL, list);
}

static void
tino_vexit(TINO_VA_LIST list)
{
  tino_vexit_n(0, list);
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
