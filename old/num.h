/* $Header$
 *
 * Number conversions
 *
 * Copyright (C)2009 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.1  2009-05-27 15:05:25  tino
 * added num.h
 *
 */

#ifndef tino_INC_num_h
#define tino_INC_num_h

#include "err.h"

#include <stdlib.h>

static unsigned long long
tino_num_secondsA(const char *arg)
{
  unsigned long long	u;
  char			*end;

  u	= strtoull(arg, &end, 0);
  if (end && *end)
    {
      unsigned long long	o, f;

      f	= 1;
      switch (*end++)
	{
	default:
	  TINO_ERR1("ETLNS101I %s unknown time suffix", arg);
	  return u;

	  /* estimates rounded up	*/
	case 'C':	f *= 36525ull; if (0)	/* Century	*/
	case 'D':	f *= 3653ull; if (0)	/* Decade=10y	*/
	case 'Y':	f *= 366ull; if (0)	/* Year		*/
	case 'S':	f *= 92ull; if (0)	/* Season	*/
	case 'M':	f *= 31ull; if (0)	/* Month	*/

	  /* exact	*/
	case 'w':	f *= 7ull;		/* Week	*/
	case 'd':	f *= 24ull;		/* Day	*/
	case 'h':	f *= 60ull;		/* Hour	*/
	case 'm':	f *= 60ull;		/* Minute	*/
	case 's':	break;			/* Seconds	*/
	}

      o	= u*f;
      if (o/f!=u)
	TINO_ERR1("ETLNS102I %s numeric overflow", arg);
      u	= o;
    }
  if (!end || *end)
    TINO_ERR1("ETLNS103I %s number not properly ended", arg);
  return u;
}

#endif
