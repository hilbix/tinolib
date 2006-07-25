/* $Header$
 *
 * Buffer line functions.
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.2  2006-07-25 20:53:04  tino
 * see ChangeLog
 *
 * Revision 1.1  2006/07/22 17:19:00  tino
 * Added untested
 *
 */

#ifndef tino_INC_buf_line_h
#define tino_INC_buf_line_h

#include "buf.h"
#include <ctype.h>

/* Scan for line terminator.
 *
 * Note that 0 (NUL) always is a line terminator!
 * c is an additional delimiter, may be -1 for blanks.
 *
 * Typical usage:
 *
 * char	*line;
 * int	pos;
 * for (pos=0; (pos=tino_buf_line_scan(buf, c, pos))>=0; )
 *   if (!add_some_more_data_to_buf(buf))
 *     if (!pos)
 *       return EOF;
 *     else
 *       break;
 * line		= tino_buf_get_s_nonconst(buf);
 * if (pos<0)
 *   pos	= -pos-1;
 * line[pos]	= 0;
 * tino_buf_advance(buf, pos+1);
 * return line;
 *
 * Returns pos>=0 for iteration.
 * Returns pos<0 for line length including(!) terminator.
 *
 * Sideeffects: calls tino_buf_advance in blanks case.
 */
static int
tino_buf_line_scan(TINO_BUF *buf, int c, int pos)
{
  int		max;
  const char	*ptr;

  max	= tino_buf_get_len(buf);
  tino_FATAL(pos>max);
  ptr	= tino_buf_get(buf);

  /* If we read spaces ignore them at start of input.
   */
  if (!pos && c<0)
    {
      while (pos<max && isspace(ptr[pos]))
	pos++;
      if (pos)
	{
	  tino_buf_advance(buf, pos);
	  return 0;
	}
    }

  /* Search for line terminator
   */
  for (; pos<max; pos++)
    if (!ptr[pos] || (c<0 ? isspace(ptr[pos]) : (unsigned char)ptr[pos]==c))
      return -pos-1;

  /* Not found, return pos
   */
  return pos;
}

/* Convenience routine:
 *
 * Read a line by a time, ignores EINTR.
 * It reads line of arbitrary length!
 *
 * For line breaking see tino_buf_line()
 *
 * Returns:
 * string	0 terminated
 * NULL		EOF or error
 */
static const char *
tino_buf_line_read(TINO_BUF *buf, int fd, int c)
{
  int	pos;
  int	n=BUFSIZ;
  char	*line;

  for (pos=0; (pos=tino_buf_line_scan(buf, c, pos))>=0; )
    {
      int	got;

      if (n<10240000)
	n	*= 2;
      got	= tino_buf_read(buf, fd, n);
      if (got>0)
	continue;
      if (!got)
	{
	  if (pos)
	    break;
	  return 0;
	}
      if (errno==EAGAIN || errno==EINTR)
	continue;
      /* Well, we cannot signal the error case.  However ignore what
       * was read up to here, it might be bullshit.
       */
      return 0;
    }
  line		= tino_buf_get_s_nonconst(buf);
  if (pos<0)
    pos	= -pos-1;
  line[pos]	= 0;
  tino_buf_advance(buf, pos+1);
  return line;
}

#endif
