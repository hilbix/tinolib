/* Fatal error handlers
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

#ifndef tino_INC_fatal_h
#define tino_INC_fatal_h

#include "ex.h"

/* This can be overwritten by exception.h
 */
#ifndef TINO_FATAL
#define	TINO_FATAL(X)	TINO_ERROR_PREFIX(fatal,X)
#define	TINO_VFATAL(X)	TINO_ERROR_PREFIX(vfatal,X)
#endif

/* This is never overwritten, it really terminates.
 */
#define	tino_FATAL(X)	do { if (X) { tino_fatal_gen(#X, __FILE__, __LINE__, __FUNCTION__); } } while(0)
#define TINO_FATAL_IF(X)	do { if (X) { TINO_FATAL(("condition: %s", #X)); } } while (0)

static void
tino_pvfatal(const char *t, TINO_VA_LIST list)
{
  tino_verror(t, list, 0);
  if (tino_exit_default_code==-1)
    tino_exit_default_code = -2;
  TINO_ABORT(tino_exit_default_code);
}

static void
tino_vfatal(TINO_VA_LIST list)
{
  tino_pvfatal("fatal error", list);
}

static void
tino_fatal(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vfatal(&list);
}

static void
tino_fatal_gen(const char *s, const char *file, int line, const char *function)
{
  tino_fatal("%s:%d:%s: %s", file, line, function, s);
}

#endif
