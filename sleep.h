/* $Header$
 *
 * Sleep functions.  Nothing is as easy as it seems ..
 * 
 * Copyright (C)2007-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.5  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.4  2007-09-26 21:09:28  tino
 * Some new functions and Cygwin fixes (started).
 *
 * Revision 1.3  2007/04/15 13:45:14  tino
 * sigsuspend() instead of select()
 *
 * Revision 1.2  2007/04/03 00:42:55  tino
 * Forgotten things corrected, thou shalt not ci untested ..
 */

#ifndef tino_INC_sleep_h
#define tino_INC_sleep_h

#include "signals.h"

#include <time.h>
#include <sched.h>

#ifdef	TINO_ALARM_RUN
#define	TINO_SLEEP_HOOK		TINO_ALARM_RUN
#else
#define	TINO_SLEEP_HOOK()
#endif

/* Sleep might be interrupted by SIGALRM or EINTR.  Also it might
 * built upon alarm().  This implementation here uses nanosleep() and
 * has no sideeffects.  It also runs the alarm callbacks and keeps the
 * watchdog quiet.
 */
static int
tino_nanosleep(time_t sec, unsigned long msec, unsigned long usec, unsigned long nsec)
{
  if (nsec>=1000000000l)
    {
      sec	+= nsec/1000000000l;
      nsec	%= 1000000000l;
    }
  if (msec)
    {
      if (msec>=1000l)
	{
	  sec	+= msec/1000l;
	  msec	%= 1000l;
	}
      nsec	+= 1000000l*msec;
    }
  if (usec)
    {
      if (usec>=1000000l)
	{
	  sec	+= msec/1000000l;
	  msec	%= 1000000l;
	}
      nsec	+= 1000l*usec;
    }
  if (nsec>=1000000000l)
    {
      sec	+= nsec/1000000000l;
      nsec	%= 1000000000l;
    }
  if (sec || nsec)
    {
      struct timespec	in, out;

      in.tv_sec	= sec;
      in.tv_nsec	= nsec;
      for (;;)
	{
	  TINO_SLEEP_HOOK();
	  if (!nanosleep(&in, &out))
	    break;
	  if (errno!=EINTR)
	    return -1;
	  TINO_SLEEP_HOOK();
	  if (!nanosleep(&out, &in))
	    break;
	  if (errno!=EINTR)
	    return -1;
	}
    }
  TINO_SLEEP_HOOK();
  return 0;
}

/** Sleep some seconds
 */
static void
tino_sleep(unsigned secs)
{
  tino_nanosleep(secs, 0l, 0l, 0l);
}

/** Sleep some milliseconds (1000 is 1 second)
 */
static void
tino_msleep(long msec)
{
  if (msec<0)
    return;
  tino_nanosleep(0l, msec, 0l, 0l);
}

/** Sleep some microseconds (1000000 is 1 second)
 */
static void
tino_usleep(long usec)
{
  if (usec<0)
    return;
  tino_nanosleep(0l, 0l, usec, 0l);
}

/** Hand over CPU to other processes
 */
static void
tino_yield(void)
{
  TINO_SLEEP_HOOK();
  sched_yield();
  TINO_SLEEP_HOOK();
}

/** Halts execution until something happens (signal)
 */
static void
tino_halt(void)
{
  TINO_SLEEP_HOOK();
  tino_sigsuspend();
  TINO_SLEEP_HOOK();
}

/** Relax process execution for 0.1s, perhaps after failures etc.
 */
static void
tino_relax(void)
{
  int	e;

  e	= errno;
  tino_yield();
  tino_nanosleep(0, 0, 0, 100000000l);	/* 0.1s	*/
  errno	= e;
}

#endif
