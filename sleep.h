/* Sleep functions.  Nothing is as easy as it seems ..
 * 
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
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
