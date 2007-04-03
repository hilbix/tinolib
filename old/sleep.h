/* $Header$
 *
 * Sleep functions.  Nothing is as easy as it seems ..
 * 
 * Copyright (C)2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.2  2007-04-03 00:42:55  tino
 * Forgotten things corrected, thou shalt not ci untested ..
 *
 * Revision 1.1  2007/04/03 00:40:34  tino
 * See ChangeLog
 */

#ifndef tino_INC_sleep_h
#define tino_INC_sleep_h

#include "alarm.h"

#include <sched.h>

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
	  tino_alarm_run_pending();
	  if (!nanosleep(&in, &out))
	    break;
	  if (errno!=EINTR)
	    return -1;
	  tino_alarm_run_pending();
	  if (!nanosleep(&out, &in))
	    break;
	  if (errno!=EINTR)
	    return -1;
	}
    }
  tino_alarm_run_pending();
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
  tino_alarm_run_pending();
  sched_yield();
  tino_alarm_run_pending();
}

/** Halts execution until something happens (signal)
 */
static void
tino_halt(void)
{
  tino_alarm_run_pending();
  select(0, NULL, NULL, NULL, NULL);
  tino_alarm_run_pending();
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
