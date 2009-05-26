/* $Header$
 *
 * Additional routines operating with auxbuf
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
 * Revision 1.1  2009-05-26 19:42:09  tino
 * added
 *
 */

#ifndef tino_INC_aux_h
#define tino_INC_aux_h

#include "auxbuf.h"

#include <time.h>
#include <sys/time.h>

/* Timestamping a la Tino: YYYYmmDD-HHMMSS.uuuuuu
 */
static const char *
tino_timestampOn(char *s, size_t max, int millis, struct timeval *tv)
{
  struct timeval	tv_tmp;
  struct tm		*tm;

  if (!tv)
    {
      gettimeofday(&tv_tmp, NULL);
      tv	= &tv_tmp;
    }
  tm	= gmtime(&tv->tv_sec);
  return tino_snprintf_ret(s, max,
			   millis ? "%04d%02d%02d-%02d%02d%02d.%06ld" : "%04d%02d%02d-%02d%02d%02d",
			   1900+tm->tm_year, 1+tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (long)tv->tv_usec);
}

static const char *
tino_aux_timestampOn(short n, int millis)
{
  return tino_timestampOn(tino_auxbufOn(n, 30), 30, millis);
}

/* Output the PID
 *
 * The PID is only read once per process to space kernel calls.  So be
 * sure to call this routine AFTER a fork(), never before!
 *
 * This IS threadsafe, as threads share the same PID.
 */
static const char *
tino_fillpidOs(char *s, size_t max)
{
  static long	mypid;

  if (!mypid)
    mypid	= getpid();
  return tino_snprintf_ret(s, max, "%ld", mypid);
}

static const char *
tino_aux_pidOs(short n)
{
  return tino_fillpidOs(tino_auxbufOn(n, 30), 30);
}

/* Typical Logging Stamp
 *
 * [YYmmDD-HHMMSS.uuuuuu][PID]
 */

/* The string must be able to accept 65 bytes.
 */
static const char *
tino_stamp65Ons(char *s65, int millis)
{
  char	*s;

  s	= s65;
  *s++	= '[';
  s	+= strlen(tino_timestampOn(s, 30, millis, NULL));
  *s++	= ']';
  *s++	= '[';
  s	+= strlen(tino_fillpidOs(s, 30));
  *s++	= ']';
  *s	= 0;
  return s65;
}

static const char *
tino_aux_logstampOns(short n)
{
  return tino_stamp65Ons(tino_auxbufOn(n, 65));
}

#endif
