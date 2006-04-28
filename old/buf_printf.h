/* $Header$
 *
 * Buffer printf now in separate include.
 * See also strprintf.h
 *
 * Copyright (C)2004-2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.1  2006-04-28 11:45:35  tino
 * va_copy now via sysfix.h (still incomplete!) and
 * buf_add_sprintf() etc. now in separate include
 *
 */

#ifndef tino_INC_buf_printf_h
#define tino_INC_buf_printf_h

#include "buf.h"
#include "sysfix.h"

#define cDP     TINO_DP_buf

static const char *
tino_buf_add_vsprintf(TINO_BUF *buf, const char *s, va_list orig)
{
  cDP(("tino_buf_add_vsprintf(%p,'%s',%ld)", buf, s, orig));
  tino_buf_add_ptr(buf, strlen(s)+1);
  for (;;)
    {
      va_list	list;
      int	out, max;

      TINO_VA_COPY(list, orig);
      max	= buf->max-buf->fill;
      out	= vsnprintf(buf->data+buf->fill, max, s, list);
      va_end(list);
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
      tino_buf_extend(buf, out);
    }
}

static const char *
tino_buf_add_sprintf(TINO_BUF *buf, const char *s, ...)
{
  const char	*ret;
  va_list	list;

  cDP(("tino_buf_add_sprintf(%p,'%s',...)", buf, s));
  va_start(list, s);
  ret	= tino_buf_add_vsprintf(buf, s, list);
  va_end(list);
  return ret;
}

#undef	cDP
#endif
