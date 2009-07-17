/* $Header$
 *
 * More advanced string helpers (like allocated sprintf).
 *
 * Copyright (C)2004-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.12  2009-07-17 00:22:10  tino
 * SOCKLINGER_PID added in env
 *
 * Revision 1.11  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.10  2008-05-19 09:14:00  tino
 * tino_alloc naming convention
 *
 * Revision 1.9  2008-01-03 00:09:38  tino
 * fixes for C++
 *
 * Revision 1.8  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.7  2007/04/11 14:55:19  tino
 * See ChangeLog
 *
 * Revision 1.6  2006/11/09 23:21:44  tino
 * Fixes
 *
 * Revision 1.5  2006/10/21 01:43:05  tino
 * va_list changes
 *
 * Revision 1.4  2006/10/04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 * Revision 1.3  2006/04/28 11:45:35  tino
 * va_copy now via sysfix.h (still incomplete!) and
 * buf_add_sprintf() etc. now in separate include
 *
 * Revision 1.2  2006/01/28 14:47:38  tino
 * pruned old things
 */

#ifndef tino_INC_strprintf_h
#define tino_INC_strprintf_h

#include "alloc.h"
#include "fatal.h"
#include "str.h"
#include "arg.h"

static char *
tino_str_vprintf_null(TINO_VA_LIST list)
{
  int	n;

  n	= BUFSIZ;
  for (;;)
    {
      char	*tmp;
      int	k;

      tmp	= (char *)tino_mallocN(n);
      if (!tmp)
	return 0;

      k	= tino_vsnprintf(tmp, n, list);
      tino_FATAL(k<0);
      if (++k<=n)
	return (char *)tino_realloc_downsizeO(tmp, k);

      tino_freeO(tmp);
      /* There is a bug in older libraries.
       * vsnprintf does not return the size needed,
       * instead it returns max.
       * We cannot distinguish between this case and the case,
       * that just the space for the \0 is missing.
       * Therefor we define to always extend buffer by BUFSIZ at minimum.
       */
      k	-= n;
      if (k<BUFSIZ)
	k	= BUFSIZ;
      n	+= k;
    }
}

static char *
tino_str_vprintf_buf(char **buf, TINO_VA_LIST orig)
{
  char	*tmp;

  tmp	= tino_str_vprintf_null(orig);
  if (!tmp)
    TINO_FATAL(("out of memory allocating string for %s", TINO_VA_STR(orig)));
  if (buf)
    *buf	= tmp;
  return tmp;
}

static char *
tino_str_vprintf(TINO_VA_LIST orig)
{
  return tino_str_vprintf_buf(NULL, orig);
}

static char *
tino_str_printf_buf(char **buf, const char *s, ...)
{
  tino_va_list	list;
  char		*tmp;

  tino_va_start(list, s);
  tmp	= tino_str_vprintf_buf(buf, &list);
  tino_va_end(list);
  return tmp;
}

static char *
tino_str_printf(const char *s, ...)
{
  tino_va_list	list;
  char		*tmp;

  tino_va_start(list, s);
  tmp	= tino_str_vprintf(&list);
  tino_va_end(list);
  return tmp;
}

#endif
