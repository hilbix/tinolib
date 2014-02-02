/* printf variants into buffer, See also strprintf.h
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

#ifndef tino_INC_buf_printf_h
#define tino_INC_buf_printf_h

#include "buf.h"
#include "arg.h"

#define cDP     TINO_DP_buf

/* XXX TODO remove following sometimes in future	*/
#define tino_buf_add_vsprintfO	tino_buf_vprintfO
#define tino_buf_add_sprintfO	tino_buf_printfO

static const char *
tino_buf_vprintfO(TINO_BUF *buf, TINO_VA_LIST list)
{
  cDP(("tino_buf_vprintf(%p,'%s',%ld)", buf, TINO_VA_STR(list), list));
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
tino_buf_printfO(TINO_BUF *buf, const char *s, ...)
{
  const char	*ret;
  tino_va_list	list;

  cDP(("tino_buf_printf(%p,'%s',...)", buf, s));
  tino_va_start(list, s);
  ret	= tino_buf_vprintfO(buf, &list);
  tino_va_end(list);
  return ret;
}

#undef	cDP
#endif
