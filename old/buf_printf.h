/* $Header$
 *
 * Buffer printf now in separate include.
 * See also strprintf.h
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
 * Revision 1.6  2008-09-01 20:18:13  tino
 * GPL fixed
 *
 * Revision 1.5  2007-09-17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.4  2007/08/08 11:26:12  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.3  2006/10/21 01:43:05  tino
 * va_list changes
 *
 * Revision 1.2  2006/10/04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 * Revision 1.1  2006/04/28 11:45:35  tino
 * va_copy now via sysfix.h (still incomplete!) and
 * buf_add_sprintf() etc. now in separate include
 */

#ifndef tino_INC_buf_printf_h
#define tino_INC_buf_printf_h

#include "buf.h"
#include "arg.h"

#define cDP     TINO_DP_buf

static const char *
tino_buf_add_vsprintfO(TINO_BUF *buf, TINO_VA_LIST list)
{
  cDP(("tino_buf_add_vsprintf(%p,'%s',%ld)", buf, TINO_VA_STR(list), list));
  tino_buf_add_ptrO(buf, strlen(TINO_VA_STR(list))+1);
  for (;;)
    {
      int	out, max;

      max	= buf->max-buf->fill;
      out	= tino_vsnprintf(buf->data+buf->fill, max, list);
      tino_FATAL(out<0);
      if (out<max)
	{
	  buf->fill	+= out;
	  return buf->data+buf->off;
	}
      /* There is a bug in older libraries.
       * vsnprintf does not return the size needed,
       * instead it returns max.
       * We cannot distinguish between this case and the case,
       * that just the space for the \0 is missing.
       * Therefor we define to always extend buffer by BUFSIZ at minimum.
       */
      out	= out-max+1;
      if (out<BUFSIZ)
	out	= BUFSIZ;
      tino_buf_extendO(buf, out);
    }
}

static const char *
tino_buf_add_sprintfO(TINO_BUF *buf, const char *s, ...)
{
  const char	*ret;
  tino_va_list	list;

  cDP(("tino_buf_add_sprintf(%p,'%s',...)", buf, s));
  tino_va_start(list, s);
  ret	= tino_buf_add_vsprintfO(buf, &list);
  tino_va_end(list);
  return ret;
}

#undef	cDP
#endif
