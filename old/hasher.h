/* $Header$
 *
 * This function shall have following probabilities:
 * A==B: for all nr: fn(nr,A)==fn(nr,B)
 * A!=B:(1) there exists a nr for which fn(nr,A)!=fn(nr,B)
 * A!=B:(2) fn(nr,A)==fn(nr,B) then with high probability fn(nr+1,A)!=fn(nr+1,B)
 * Well, we could use a crypto function for this, however this is slow.
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
 * Revision 1.6  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.5  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.4  2005/06/04 14:08:09  tino
 * include plus minor fix
 *
 * Revision 1.3  2005/03/04 00:58:56  tino
 * Old hash function was poor
 *
 * Revision 1.2  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/04/20 23:51:38  tino
 * Hashing added (untested!)
 */

#ifndef tino_INC_hasher_h
#define tino_INC_hasher_h

#include <stdio.h>

/* Well, I would like to do
 * sum*=n
 * sum+=ptr[i]
 * as this *must* fulfill what I wrote above (mathematically, not numerically).
 * But this is dead slow (multiplication).
 */
static long
hasher(const unsigned char *ptr, size_t len, int nr)
{
  long	sum;
  int	i;

  if (!nr)
    return (((int)ptr[0])<<8)|ptr[len-1];
  sum	= 0;
  for (i=len; --i>=0; )
    {
#if 0
      sum	+= nr;
      sum	^= ptr[i];
#else
      sum	*= nr;
      sum	+= ptr[i];
#endif
    }
  return sum;
}

#endif
