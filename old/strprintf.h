/* $Header$
 *
 * More advanced string helpers (like allocated sprintf).
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
 * Revision 1.2  2006-01-28 14:47:38  tino
 * pruned old things
 *
 */

#ifndef tino_INC_strprintf_h
#define tino_INC_strprintf_h

#include "fatal.h"
#include "str.h"

static char *
tino_str_vprintf_null(const char *s, va_list orig)
{
  int	n;

  n	= BUFSIZ;
  for (;;)
    {
      va_list	list;
      char	*tmp;
      int	k;

      tmp	= (char *)malloc(n);
      if (!tmp)
	return 0;

      va_copy(list, orig);
      k	= vsnprintf(tmp, n, s, list);
      va_end(list);
      tino_FATAL(k<0);
      if (++k<=n)
	{
	  realloc(tmp, k);
	  return tmp;
	}
      free(tmp);
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
tino_str_vprintf(const char *s, va_list orig)
{
  char	*tmp;

  tmp	= tino_str_vprintf_null(s, orig);
  if (!tmp)
    TINO_FATAL(("out of memory allocating string for %s", s));
  return tmp;
}

static char *
tino_str_printf(const char *s, ...)
{
  va_list	list;
  char		*tmp;

  va_start(list, s);
  tmp	= tino_str_vprintf(s, list);
  va_end(list);
  return tmp;
}

#endif
