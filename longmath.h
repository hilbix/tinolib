/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * Mathematics with 'unsigned long long' or similar
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.3  2008-11-02 00:02:07  tino
 * -
 *
 * Revision 1.2  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.1  2006-07-31 23:20:18  tino
 * intermediate version which is not ready yet
 */

#ifndef tino_INC_longmath_h
#define tino_INC_longmath_h

#include <stdio.h>

/* Get a fractional number with an offset.
 *
 * Returns 0 on success, -1 on syntax error, 1 on overrun, -2 on other
 * errors.  If end is NULL then the string *must* be fully parsed,
 * else a syntax error is signalled, else end is set to the first
 * nonparseable character.
 *
 * The number is something like [~|-|+][base]number.number
 *
 * The .number can have at most the given number of digits.
 *
 * If digits==0 then the . can be present and the remainder is
 * ignored.  If digits<0 then the . must not be present.  Note that
 * the number is multiplied with the number of digits according to the
 * base.  Note that if you don't know the number of digits in case
 * base is not predefined.
 *
 * This can be done on an arbitrary base, like 0 (detect), 1 (binary),
 * 8 (octal), 10 (decimal) or even 16 (hex).  If base is NULL base is
 * fixed to 10.  If *base==0 the base is taken from a prefix.  This is
 *
 * ! or z or 0z 64 base 64 compatible
 * # or y or 0y 32 double hex (0-9 and a-q or A-Q)
 * $ or x or 0x	16 hex
 * % or o or 0o	8 octal
 * ^ or q or 0q	4 quad
 * & or b or 0b	2 binary
 *
 * Hint: This is Shift 1 to Shift 7 on the US keyboard.  The following
 * is Shift 8 or Shift 9/Shift 0 on the US keyboard:
 *
 * * or d or 0d 10 decimal
 * (n)		n, any base from 2 to 64
 *
 * The sign is only checked if sign is not NULL.  In this case sign is
 * set: 0 on ~, -1 on -, 1 on +, unchanged if no sign is present.
 * Watch out for the 0 case!  You must calculate the negative or
 * inverse value yourself!
 */

static int
tino_longmath_get_base(const char **arg)
{
  const char	*p;
  int		n;

  /* hopefully the compiler optimizes this into something better
   */
  p	= *arg;
  switch (*p)
    {
    case '(':
      n	= strtoul(p+1, &p, 0);
      if (n<2 || n>64 || *p!=')')
	return 10;
      break;

    case '!':	n=64; break;
    case '#':	n=32; break;
    case '$':	n=16; break;
    case '%':	n=8; break;
    case '^':	n=4; break;
    case '&':	n=2; break;
    case '*':	n=10; break;

    case '0':
      p++;
    default:
      switch (*p)
	{
	default:
	  return 10;

	case 'z': case 'Z':	n=64; break;
	case 'y': case 'Y':	n=32; break;
	case 'x': case 'X':	n=16; break;
	case 'o': case 'O':	n=8; break;
	case 'q': case 'Q':	n=4; break;
	case 'b': case 'B':	n=2; break;
	case 'd': case 'D':	n=10; break;
	}
    }
  *arg	= p+1;
  return n;
}

static int
tino_longmath_fractional_const(const char *arg, unsigned long long *val, const char **end, int *sign, int *base, int digits)
{
  int			n;
  unsigned long long	v;

  if (!arg || !val)
    return -2;

  /* get the sign	*/
  if (sign)
    switch (*arg)
      {
      case '-':	*sign	= -1;	if (0)
      case '+':	*sign	= +1;	if (0)
      case '~':	*sign	= 0;
	arg++;
	break;
      }

  /* get the base	*/
  n	= 10;
  if (base && (n= *base)==0)
    n	= tino_longmath_get_base(&arg);
  if (n<2 || n>64)
    return -2;
  if (base && *base!=n)
    *base	= n;

  /* build the number	*/
  v	= 0;
  do
    {
      int	k;

      k	= *sign;
      v	= v*n+k;
    } while (*arg);

  000;
}

static int
tino_longmath_fractional(char *arg, unsigned long long *val, const char **end, int *sign, int *base, int digits)
{
  return tino_longmath_fractional_const(arg, val, (const char **)end, sign, base, digits);
}

#endif
