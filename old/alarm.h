/* $Header$
 *
 * Alarm list processing.
 * 
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.2  2007-01-25 04:39:15  tino
 * Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
 *
 * Revision 1.1  2006/10/21 01:42:01  tino
 * added
 *
 */

#ifndef tino_INC_alarm_h
#define tino_INC_alarm_h

#include "fatal.h"
#include "alloc.h"

#include <time.h>
#include <signal.h>

/** Private structure
 */
struct tino_alarm_list
  {
    struct tino_alarm_list	*next;
    int				seconds;
    time_t			stamp;
    int				(*cb)(void *, long);
    void			*user;
  };

/** Private variables
 */
static struct tino_alarm_list	*tino_alarm_list_active, *tino_alarm_list_inactive;
static int			tino_alarm_pending;
static int			tino_alarm_watchdog;

/** Private alarm handler
 */
static void
tino_alarm_handler(int sig)
{
  tino_alarm_pending++;
  if (tino_alarm_watchdog)
    {
      if (tino_alarm_pending>tino_alarm_watchdog)
	tino_fatal("watchdog");
#ifdef TINO_USE_NO_SIGACTION
      if (signal(SIGALRM, tino_alarm_handler))
	tino_fatal("signal");
#endif
      alarm(1);
    }
}

/** Public run alarm shortcut
 *
 * Call this macro to run the pending alarms.  Do this after routines
 * (like read() and write()) which can return EINTR.
 */
#define	TINO_ALARM_RUN()	do { if (tino_alarm_pending) tino_alarm_run(); } while (0)

/** Set the watchdog, disabled when 0
 *
 * The watchdog is something which hits, if the alarm is pending
 * longer than the given seconds and it is not yet processed.  If
 * there is no active alarm, the watchdog runs each second, such that
 * you must call TINO_ALARM_RUN() regularily.
 *
 * There is no watchdog action (yet).  It's always fatal.
 */
static void
tino_alarm_set_watchdog(int watchdog)
{
  tino_alarm_watchdog	= watchdog;
}

/** Sort in new alarms
 *
 * XXX, TODO: Leave improvements to future.
 */
static void
tino_alarm_sort( struct tino_alarm_list *tmp)
{
  while (tmp)
    {
      struct tino_alarm_list	*ptr, **last;
      time_t			stamp;

      for (last= &tino_alarm_list_active; (ptr= *last)!=0 && ptr->stamp<tmp->stamp; last= &ptr->next);
      *last	= tmp;
      stamp	= tmp->stamp;
      do
	{
	  last	= &tmp->next;
	} while ((tmp= *last)!=0 && tmp->stamp==stamp);
      *last	= ptr;
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
  time_t			now;
  long				delta;

  alarm(0);
  tino_alarm_pending	= 0;
  time(&now);

  /* Run the alarms up to now
   */
  for (last= &tino_alarm_list_active; (ptr= *last)!=0 && (delta=now-ptr->stamp)>=0; )
    {
      ptr->stamp	= now+ptr->seconds;
      if (ptr->cb(ptr->user, delta))
	{
	  *last				= ptr->next;
	  ptr->next			= tino_alarm_list_inactive;
	  tino_alarm_list_inactive	= ptr;
	}
      else
	last= &ptr->next;
    }
  *last				= 0;

  tmp				= tino_alarm_list_active;
  tino_alarm_list_active	= ptr;

  tino_alarm_sort(tmp);

  /* Schedule next alarm
   */
  if (tino_alarm_list_active || tino_alarm_watchdog)
    {
#ifdef TINO_USE_NO_SIGACTION
      if (signal(SIGALRM, tino_alarm_handler))
	tino_fatal("signal");
#endif
      delta	= tino_alarm_list_active ? now-tino_alarm_list_active->stamp : 1;
      if (delta<1)
	delta	= 1;
      if (delta>1000)
	delta	= 1000;
      alarm(delta);
    }
}

/** Stop an alarm callback
 *
 * Searches callback/user in alarm list and deletes it.
 *
 * If callback or user is NULL, any value will fit.
 *
 * tino_alarm_stop(NULL,NULL) stops all alarms.
 */
static void
tino_alarm_stop(int (*callback)(void *, long), void *user)
{
  struct tino_alarm_list	**last, *ptr;

  for (last= &tino_alarm_list_active; (ptr= *last)!=0; )
    {
      if ((callback && ptr->cb!=callback) ||
	  (user && ptr->user!=user))
	{
	  last	= &ptr->next;
	  continue;
	}
      *last			= ptr->next;
      ptr->next			= tino_alarm_list_inactive;
      tino_alarm_list_inactive	= ptr;
    }
}

/** Set an alarm callback
 *
 * The alarm callback function will be run each seconds with the user arg.
 *
 * The second argument to the callback function is the number of
 * seconds elapsed after the alarm() had to run (hopefully always 0).
 */
static void
tino_alarm_set(int seconds, int (*callback)(void *, long), void *user)
{
  struct tino_alarm_list	*ptr;
  time_t			now;

  tino_alarm_stop(callback, user);

  if ((ptr=tino_alarm_list_inactive)==0)
    ptr	= tino_alloc0(sizeof *ptr);
  else
    tino_alarm_list_inactive	= ptr->next;

  time(&now);
  ptr->seconds	= seconds;
  ptr->stamp	= now+seconds;
  ptr->cb	= callback;
  ptr->user	= user;

#ifndef TINO_USE_NO_SIGACTION
  if (!tino_alarm_list_active)
    {
      struct sigaction sa;

      memset(&sa, 0, sizeof sa);
      sa.sa_handler	= tino_alarm_handler;

      if (sigaction(SIGALRM, &sa, NULL))
	tino_fatal("sigaction");
    }
#endif

  tino_alarm_sort(ptr);
  tino_alarm_run();
}

#endif
