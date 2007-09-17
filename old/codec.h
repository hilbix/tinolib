/* $Header$
 *
 * Simple coder / decoders.
 * Sometimes perhaps replace this by a complete "real" codec layer.
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.4  2007-09-17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.3  2006/10/03 20:37:13  tino
 * Ubuntu fixes
 *
 * Revision 1.2  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.1  2004/04/08 21:39:00  tino
 * New HEX input functions
 */

#ifndef tino_INC_codec_h
#define tino_INC_codec_h

#include "sysfix.h"

static TINO_INLINE int
tino_dec_hex_digitO(char c)
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
  int	i;
  unsigned char	*_p=p;

  for (i=0; i<len; i++)
    {
      int	c;

      c	= tino_dec_hex_digitO(*hex++)<<4;
      c	|= tino_dec_hex_digitO(*hex++);
      if (c<0)
	return i;
      *_p++	= c;
    }
  return i;
}

#endif
