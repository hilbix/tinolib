/* Simple coder / decoders.
 * Sometimes perhaps replace this by a complete "real" codec layer.
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

#ifndef tino_INC_codec_h
#define tino_INC_codec_h

#include "sysfix.h"

TINO_INLINE(static int
tino_dec_hex_digitO(char c))
{
  switch (c)
    {
    case '0':	return 0;
    case '1':	return 1;
    case '2':	return 2;
    case '3':	return 3;
    case '4':	return 4;
    case '5':	return 5;
    case '6':	return 6;
    case '7':	return 7;
    case '8':	return 8;
    case '9':	return 9;
    case 'a':	return 10;
    case 'b':	return 11;
    case 'c':	return 12;
    case 'd':	return 13;
    case 'e':	return 14;
    case 'f':	return 15;
    }
  return -1;
}

/* Decode at max len bytes of a hex string into a buffer
 * Return number decoded, else error.
 * if n*2==strlen(hex) decoding is complete.
 */
static int
tino_dec_hexO(void *p, int len, const char *hex)
{
  int		i;
  unsigned char	*_p=(unsigned char *)p;

  for (i=0; i<len; i++)
    {
      int	c;

      c	= tino_dec_hex_digitO(*hex++)<<4;
      if (c<0)
        return i;
      c	|= tino_dec_hex_digitO(*hex++);
      if (c<0)
	return i;
      *_p++	= c;
    }
  return i;
}

#endif
