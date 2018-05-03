/* Alarm list processing.  This is slow alarm clocks with roughly a
 * second precision (note that 1 second may become 2).
 *
 * Important notes:
 * - Multithreading is not supported and leads to unpredictable results.
 * - You must not use longjmp() (like exceptions) in callbacks.
 *
 * Copyright (C)2006-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_alarm_h
#define tino_INC_alarm_h

#ifdef	tino_INC_file_h
#error	"For valid EINTR processing, alarm.h must be included before file.h"
#endif

/** Public run alarm shortcut
 *
 * Call this macro to run the pending alarms.  Do this after routines
 * (like read() and write()) which can return EINTR.
 */
#define	TINO_ALARM_RUN()	do { if (tino_alarm_pending) tino_alarm_run(); } while (0)
static int			tino_alarm_pending, tino_alarm_synthetics;
static void tino_alarm_run(void);

#include "file.h"	/* must be included, does not work else	*/
#include "fatal.h"
#include "alloc.h"
#include "signals.h"

#include <time.h>

/** Private structure
 */
struct tino_alarm_list
  {
    struct tino_alarm_list	*next;
    int				seconds;
    time_t			stamp, started, run;
    int				(*cb)(void *, long, time_t, long);
    void			*user;
  };

/** Private variables
 */
static struct tino_alarm_list	*tino_alarm_list_active, *tino_alarm_list_inactive;
static int			tino_alarm_running;
static int			tino_alarm_watchdog;

/** Wrapper to library (just in case it's buggy)
 */
static void
tino_alarm(unsigned secs)
{
  TINO_F_alarm(secs);
}

/** Private alarm handler
 *
 * As there is a race in select() (and others), re-issue the alarm
 * each second until it is honored.
 */
static void
tino_alarm_handler(void)
{
  if (!tino_alarm_running && !tino_alarm_synthetics)
    return;

  if (++tino_alarm_pending>tino_alarm_watchdog && tino_alarm_watchdog)
    tino_fatal("watchdog");

  TINO_SIGNAL(SIGALRM, tino_alarm_handler);
  tino_alarm(1);
}

/** Sort in new alarms
 */
static void
tino_alarm_sort(struct tino_alarm_list *add)
{
  while (add)
    {
      struct tino_alarm_list	*ptr, **last;

      for (last= &tino_alarm_list_active;; last= &ptr->next)
	{
	  /* Hunt for position to insert.
	   *
	   * New alarms will be appended to all other alarms with the
	   * same timestamp.
	   *
	   * The funny compare (by substraction) is failproof when
	   * time_t runs over.
	   */
	  if ((ptr= *last)!=0 && ((long)(ptr->stamp-add->stamp))>=0)
	    continue;

	  /* Insert element at current position
	   */
	  ptr		= add;
	  add		= add->next;
	  ptr->next	= *last;
	  *last		= ptr;

	  /* Are we ready?
	   */
	  if (!add)
	    return;

	  /* If the sequence is broken, that is the next element to
	   * add (*add) must fire before the current added element
	   * (*ptr), then start from all over.
	   */
	  if (((long)(ptr->stamp-add->stamp))<0)
	    break;
	}
    }
}

/** Run alarms
 *
 * If alarm function returns nonzero, the alarm calls will be disabled.
 *
 * Convention:
 * 1	one time alarm
 * 0	continuous alarm
 * -1	error in routine (stop alarm)
 */
static void
tino_alarm_run(void)
{
  struct tino_alarm_list	*tmp, *ptr, **last;
  static time_t			reference;
  time_t			now;
  long				delta;
  int				wasrunning;
  int				e;

  e			= errno;

  wasrunning		= tino_alarm_running;
  tino_alarm_pending	= 0;
  tino_alarm_synthetics	= 0;
  time(&now);

  /* If time runs backward (because you set the time) the alarms must
   * be corrected.
   */
  if (reference && (delta=reference-now)>0)
    for (ptr=tino_alarm_list_active; ptr; ptr=ptr->next)
      ptr->stamp	-= delta;
  reference	= now;
  if (!reference)	/* Am I paranoid!	*/
    reference--;

  /* Run the alarms up to now
   */
  for (last= &tino_alarm_list_active; (ptr= *last)!=0 && (delta=now-ptr->stamp)>=0; )
    {
      int	ret;

      /* Avoid reentry of the already active alarm callback
       *
       * (You cannot use longjmp() in alarm callbacks.)
       */
      if (ptr->run)
	{
	  last	= &ptr->next;
	  continue;
	}

      ptr->stamp	= now + (ptr->seconds>=0 ? ptr->seconds : -ptr->seconds);
      ptr->run		= 1;
      ret		= (ptr->cb ? ptr->cb(ptr->user, delta, now, now-ptr->started) : !!ptr->user);
      if (ret || ptr->run<0)
	{
	  /* alarm was running while tino_alarm_stop() was called
	   */
	  *last				= ptr->next;
	  ptr->next			= tino_alarm_list_inactive;
	  tino_alarm_list_inactive	= ptr;
	}
      else
	last= &ptr->next;
      ptr->run		= 0;
    }
  *last				= 0;
  tmp				= tino_alarm_list_active;
  tino_alarm_list_active	= ptr;

  tino_alarm_sort(tmp);

  delta	= 0;
  for (ptr=tino_alarm_list_active; ptr; ptr=ptr->next)
    if (ptr->seconds>0)
      {
        delta	= ptr->stamp-now;
	if (!delta)
	  delta	= 1;
        break;
      }
  if (!delta)
    delta	= tino_alarm_watchdog;

  /* Schedule next alarm
   */
  if (delta)
    {
      TINO_SIGNAL(SIGALRM, tino_alarm_handler);

      if (delta<1)
	delta	= 1;
      if (delta>1000)
	delta	= 1000;
      tino_alarm_running	= delta;
      tino_alarm(delta);
    }
  else if (wasrunning)
    {
      tino_alarm_running	= 0;
      tino_alarm(0);
    }

  errno	= e;
}

/** Set the watchdog, disabled when 0
 *
 * The watchdog is something which hits, if the alarm is pending
 * longer than the given seconds and it is not yet processed.  If
 * there is no active alarm, the watchdog runs after the watchdog
 * interval (thus the watchdog period is doubled), such that you can
 * process long running options.  You must call TINO_ALARM_RUN()
 * regularily, else the watchdog bites!
 *
 * (Previously the watchdog ran each second, this now is extended to
 * the watchdog-interval.  The watchdog still bites, only a little
 * later).
 *
 * There is no watchdog action (yet).  It's always fatal.  Using
 * watchdog time of 1s probably is not wise.
 */
static void
tino_alarm_set_watchdog(int watchdog)
{
  tino_alarm_watchdog	= watchdog;
  tino_alarm_run();	/* switch watchdog on and off	*/
}

/** Run alarms if pending
 *
 * Wrapper function
 */
static void
tino_alarm_run_pending(void)
{
  TINO_ALARM_RUN();
}

/** Check if an alarm is pending
 *
 * This is the correct interface to access tino_alarm_pending.
 */
static int
tino_alarm_is_pending(void)
{
  return tino_alarm_pending;
}

/** Stop an alarm callback
 *
 * Searches callback/user in alarm list and deletes it.
 *
 * If user is NULL, any value will fit.
 *
 * Important Change:
 *
 * tino_alarm_stop(NULL,NULL) no more stops all alarms.  Use
 * tino_alarm_stop_all() for this!
 *
 * Note that this does not re-schedule alarms.
 */
static void
tino_alarm_stop(int (*callback)(void *, long, time_t, long), void *user)
{
  struct tino_alarm_list	**last, *ptr;

  for (last= &tino_alarm_list_active; (ptr= *last)!=0; )
    {
      if (ptr->cb!=callback || (callback && user && ptr->user!=user))
	{
	  last	= &ptr->next;
	  continue;
	}
      if (ptr->run)
	ptr->run	= -1;	/* alarm currently running, mark for stop on return	*/
      else
	{
	  *last				= ptr->next;
	  ptr->next			= tino_alarm_list_inactive;
	  tino_alarm_list_inactive	= ptr;
	}
    }
  if (!tino_alarm_list_active)
    tino_alarm_run();		/* reschedule the watchdog	*/
}

/** Stop all alarms
 *
 * If called twice this frees all the alarm structures.  It does not
 * unregister the signal handler functions, though.
 *
 * Note: If you want to disable the watchdog, too, use following
 * sequence:
 *
 * tino_alarm_set_watchdog(0);
 * tino_alarm_stop_all();
 * tino_alarm_stop_all();
 *
 * This also shall ensure that no more spurious alarms show up in
 * future.
 */
static void
tino_alarm_stop_all(void)
{
  struct tino_alarm_list	*ptr;

  while ((ptr=tino_alarm_list_inactive)!=0)
    {
      tino_alarm_list_inactive	= ptr->next;
      free(ptr);
    }
  tino_alarm_list_inactive	= tino_alarm_list_active;
  tino_alarm_list_active	= 0;

  tino_alarm_run();	/* Disable alarm()s	*/
}

/** Set an alarm callback
 *
 * The alarm callback function will be run each seconds.
 *
 * If seconds is not > 0, it does not schedules alarms.
 * Instead it runs at most each -seconds time when some other alarm runs.
 * Note that there is no particular order, hence it may run before or
 * after the alarm, which triggered.
 *
 * The callback function must return 0 to continue to run, else it
 * will be stopped.  Arguments to callback function:
 *
 * 1: the user pointer
 * 2: The number of seconds elapsed after the alarm() had to run
 * (hopefully 0)
 * 3: The current time stamp (spares a call to time())
 * 4: The number of seconds since the alarm was set
 *
 * If callback is NULL this is an anonymous alarm.  If user is NULL,
 * too, this is continuously generating EINTR signals until you stop
 * it, else it will just run one time.  Note that you can only set one
 * such alarm (this is to prevent table fills in case of errors)!
 */
static void
tino_alarm_set(int seconds, int (*callback)(void *user, long delta, time_t now, long run), void *user)
{
  struct tino_alarm_list	*ptr;
  time_t			now;

  tino_alarm_stop(callback, user);

  if ((ptr=tino_alarm_list_inactive)==0)
    ptr	= (struct tino_alarm_list *)tino_alloc0O(sizeof *ptr);
  else
    tino_alarm_list_inactive	= ptr->next;

  time(&now);
  ptr->seconds	= seconds;
  ptr->stamp	= now+seconds;
  ptr->started	= now;
  ptr->run	= 0;
  ptr->cb	= callback;
  ptr->user	= user;

  if (!tino_alarm_list_active && !tino_alarm_list_inactive)
    TINO_SIGACTION(SIGALRM, tino_alarm_handler);

  tino_alarm_sort(ptr);
  tino_alarm_run();
}

/** Produce a synthetic alarm
 *
 * This triggers tino_alarm_set(0, ...) callbacks.
 *
 * I am not happy with this hack here.  There should not be synthetic alarms.
 *
 * What we probably need are: triggers.
 *
 * - Register trigger callbacks.
 * - Attach signals and alarms to those triggers.
 * - Multiple signals and multiple alarms can be attached to a single trigger.
 * - Multiple triggers can be attached to the same signal or alarm.
 * - Triggers also can be called directly.
 * - When a trigger is called, it receives a list of structures of which signals/alarms to process.
 *
 * Implement this in tino/triggers.h
 *
 * - Register and deregister triggers.
 * - Associate signals to those triggers.
 * - Associate alarms to those triggers.
 * - Create a scheduler which calls those triggers.
 * - TINO_ALARM_RUN() then calls this scheduler.
 * - Perhaps implement setitimer() as well.
 */
static void
tino_alarm_syntheticO(void)
{
  if (!tino_alarm_pending)
    {
      tino_alarm_synthetics = 1;
      tino_alarm_handler();
    }
}

#endif
