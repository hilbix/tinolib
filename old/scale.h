/* $Header$
 *
 * Various value scaling
 *
 * Copyright (C)2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.1  2008-10-19 22:23:39  tino
 * Added
 *
 */

#ifndef tino_INC_scale_h
#define tino_INC_scale_h

#include "auxbuf.h"

#include <string.h>

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static size_t
tino_scale_max(int a, int b)
{
  if (a<0)
    a	= -a;
  if (b<0)
    b	= -b;
  return a<b ? b : a;
}

static char *
tino_scale_buf(size_t *len, short n, int width)
{
  return tino_auxbufOn(n, (*len = tino_scale_max(20, width)+1));
}

static const char *
tino_scale_ret(char *buf, int width)
{
  size_t	len;
  char		fill;
  int		i;

  fill	= '0';
  if (width<0)
    {
      width	= -width;
      fill	= ' ';
    }
#if 0
  tino_FATAL(max<width);
#endif
  len	= strlen(buf);
  if (len>=width)
    return buf;

  i	= width-len;
  memmove(buf+i, buf, len+1);
  while (--i>=0)
    buf[i]	= fill;
  return buf;
}

static int
tino_scale_number(char *buf, size_t len, unsigned long long var, unsigned long long scale, int width, int trail)
{
  int			pos, max;
  unsigned long long	tmp;

  if (!scale)
    scale	= 1;
  snprintf(buf, len, "%llu", var/scale);

  pos	= strlen(buf);

  max	= width;
  if (max<0)
    max	= -max;
  max	-= trail+1;

  if (pos>=max)
    return pos;

  buf[pos]= '.';
  tmp	= scale;
  while (tmp>1 && pos<max)
    {
      var	%= scale;
      tmp	/= 10;
      var	*= 10;
      buf[++pos]= '0'+(var/scale);
    }

  return pos+1;
}

static const char *
tino_scale_byte_helper(short n, unsigned long long bytes, int minext, int width, unsigned long long scale)
{
  static const char	ext[]=" KMGTPEZY";
  char			*buf;
  size_t		len;
  int			i, pos;

  if (!scale)
    scale	= 1;

  for (i=0; (i<minext || bytes>=10000*scale) && ext[i+1]; i++)
    scale	*= 1024;

  buf	= tino_scale_buf(&len, n, width);

  pos	= tino_scale_number(buf, len, bytes, scale, width, ext[i]==' ' ? 0 : 1);

  if (ext[i]!=' ')
    {
      buf[pos]	= ext[i];
      buf[++pos]= 0;
    }
  return tino_scale_ret(buf, width);
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/** Time scaling
 */
static const char *
tino_scale_interval(short n, long sec, int minfields, int width)
{
  char		*buf;
  size_t	len;

  buf	= tino_scale_buf(&len, n, width);

  if (sec<1000 && minfields<1)
    snprintf(buf, len, "%ld", sec);
  else if (sec<6000 && minfields<2)
    snprintf(buf, len, "%ld:%02ld", sec/60, sec%60);
  else if (sec<360000 && minfields<3)
    snprintf(buf, len, "%ld:%02ld:%02ld", sec/3600, (sec/60)%60, sec%60);
  else
    snprintf(buf, len, "%ld-%02ld:%02ld:%02ld", sec/24/3600, (sec/3600)%24, (sec/60)%60, sec%60);

  return tino_scale_ret(buf, width);
}

/** Byte scaling.
 *
 * Scale a byte value into width bytes with minimum extension (0=byte,
 * 1=KB ..).
 */
static const char *
tino_scale_bytes(short n, unsigned long long bytes, int minext, int width)
{
  return tino_scale_byte_helper(n, bytes, minext, width, 1ull);
}

/** Throughput (Byte/s) scaling
 */
static const char *
tino_scale_speed(short n, unsigned long long bytes, unsigned long long interval, int minext, int width)
{
  return tino_scale_byte_helper(n, bytes, minext, width, interval);
}

static const char *
tino_scale_percent(short n, unsigned long long val, unsigned long long total, int width)
{
  char		*buf;
  size_t	len;

  buf		= tino_scale_buf(&len, n, width);

  tino_scale_number(buf, len, val*100, total, width, 0);

  return tino_scale_ret(buf, width);
}

#endif
