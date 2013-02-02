/* $Header$
 *
 * NOT READY YET *
 * NOT THREAD SAFE *
 * UNIT TEST FAILS *
 *
 * Scheduler mechanisms
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
 * Revision 1.2  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.1  2007-04-16 19:52:21  tino
 * See ChangeLog
 */

#ifndef tino_INC_sched_h
#define tino_INC_sched_h

/** A scheduler is something, which decides what to do next.
 *
 * This here shall be as generic as possible, so following things
 * shall go in:
 *
 * - Can be called any time and returns a pointer to the next
 * scheduler object.  This can be the current one in case that nothing
 * changed a lot.
 *
 * - Scheduler objects have a precedence rating and a priority level.
 *
 * - Scheduler objects have a type, can be inactive or active, limited
 * or permant, suspended or scheduled.  Inactive objects are never
 * considered by the scheduler (you may see this as a blocked
 * process).  The opposit is active (a runnable process).  Limited is,
 * the object only is chosen a limited time from the scheduler until
 * it becomes inactive, the opposit is permanent.  Suspended objects
 * are objects, which are not scheduled (waiting process).  Scheduled
 * objects are those objects, which were handed out by the scheduler
 * (running processes).
 *
 * - The scheduler can hand out as many objects as are active.  To
 * suspend an object again, the object must be handed back to the
 * scheduler (so the scheduler has no means of "forcably stopping a
 * task", the task must be cooperative).  Note that such a suspended
 * object, if active, may be handed out immediately again.
 *
 * - Scheduler objects are handed out according to the precedence
 * first, then are scheduled according to the priority and at the
 * lowest level are just scheduled round robin.  The priority of an
 * object can be (temporarily) modified by the precedence algorithm
 * according to the type of the object.
 *
 * Precedence calculation is very easy to understand:
 *
 * All current active precedence levels are summed up (result is S)
 * and an object with precedence N then has N/S probability to be
 * chosen.  This is, if precedence 2 has 2 objects and precedence 1
 * has 1 object, the scheduler selects each objects in turn.  If
 * precedence 2 only has one object, then this object is chosen two
 * times before the object with precedence 1 is chosen a single time.
 * So from level 60 objects get selected at a 30 times higher rate
 * than at precedence level 2 and 20 times higher rate than object
 * with precedence level 3.
 *
 * Please note that this method means, that if in level 60 are 60
 * objects and in level 2 is only one object, the object in level 2 is
 * selected twice as often as a single object from level 60 (because
 * after 30 objects of level 60 one object of level 2 is considered,
 * this is the single one at level 2).
 *
 * There also is a precedence 0. From this level no objects are chosen
 * only, if no other object can be chosen.  This is a true "null task
 * level", where something only runs if there is nothing else to run.
 *
 * Negative precedence levels mean, the higher the (absolute) value
 * is, the more this object takes precedence.  This is, if there are
 * objects in precedence level -2, -1 and 5, first all objects in
 * level -2 are chosen until there are no more active and suspended
 * objects in this level, then the objects from level -1 are selected
 * until there are no more (again) and then the normal scheduling is
 * back in place.  You can see negative precedences as "real time
 * tasks" which always interrupt anything else.
 *
 * Within a precedence level, the priority selects, which object is
 * chosen.  This is done based on the priority value.  Each time an
 * object is not chosen, it's priority value is added to the priority.
 * The object with the highest priority then is selected (and the
 * priority value is not incremented).  As a formula this looks like:
 *
 * Co = (N-No)*Po-Do
 *
 * where Co is the priority to compare to others, N is the number of
 * objects handed out from the precedence level, No counts how often
 * the object was choosen already and Po is the priority value of the
 * object.  New objects always start with the lowest priority value
 * from the last schedling.  This is, they are below the current
 * priority of all other objects.
 *
 * Do is a modifier which can be incremented at will.  For example, if
 * the prioritization shall be done according to the time slices, the
 * (rounded up) milliseconds since the last scheduling can be added to
 * the Do of an object, thus lowering the priority of it.
 *
 * Priority values can be negative and even zero, this does not change
 * anything.  If there are objects of equal priority, this leads to
 * round robin, as the scheduled object is moved at the end of the
 * list after it was handed out.
 */

typedef struct tino_scheduler	tino_scheduler;
enum tino_sched_action
  {
    TINO_SCHED_FREE=0,		/* Free the object structures from memory	*/
    TINO_SCHED_NEW,		/* Init an unitialized object structure		*/
    TINO_SCHED_INIT,		/* Init an already initialized object structure	*/
    TINO_SCHED_SCHEDULED,	/* Object is scheduled				*/
    TINO_SCHED_SUSPEND,		/* Object is suspended				*/
    TINO_SCHED_WALK,		/* Houskeeping callback				*/
    TINO_SCHED_CLEAN,		/* Cleanup object which is no more used		*/
  };
typedef	void			(*tino_scheduler_cb)(tino_scheduler *sch, int ob, enum tino_sched_action, void *user);

struct tino_schedob
  {
    tino_sched_ob	next, *last;	/* scheduling list	*/

    /* Current state	*/
    int			state;
    int			active;

    /* Settings from user	*/
    int			type;
    void		*user;
    int			prec, prio;
    int			limit;
  };

struct tino_scheduler
  {
    tino_scheduler_cb	cb;
    void		*cb_user;
    struct tino_schedob	*q_act, *q_inact;
  };

/** Set a new scheduler callback function and return the old one.
 *
 * Warning: the parameters are modified to the old values.
 *
 * If you give NULL as parameters this cores.  This is intended.
 */
static void
tino_sched_cb_add(tino_scheduler *sch, tino_scheduler_cb **callback, void **user)
{
  tino_scheduler_cb	old;
  void			*old_user;

  old		= sch->callback;
  old_user	= sch->callback_user;
  sch->cb	= *callback;
  sch->cb_user	= *user;
  *callback	= old;
  *user		= old_user;
}

/** Convenience routine: Set a new scheduler callback function
 */
static void
tino_sched_cb_set(tino_scheduler *sch, tino_scheduler_cb callback, void *user)
{
  tino_sched_cb_add(sch, &callback, &user);
}

/** Convenience routine: New scheduler objects must be nulled.
 *
 * You can declare global static scheduler structures.  However if you
 * do this dynamically on the stack, be sure that it is nulled.  This routine is 
 * not, you have a problem.
 */
static void
tino_sched_init(tino_scheduler *sch, tino_scheduler_cb callback, void *user)
{
  memset(sch, 0, sizeof *sch);
  tino_sched_cb_set(sch, callback, user);
}

/** Internal routine, do not use
 */
static void
tino_sched_statechange(tino_scheduler *sch, int ob, int state)
{
  if (sch->cb)
    sch->cb(sch, ob, state, sch->user);
  sch->ob[ob].state	= state;
}

/** Internal routine, do not use
 */
static void
tino_sched_free_ob(tino_scheduler *sch, int ob)
{
  switch (sch->ob[ob]->state)
    {
    default:
      tino_sched_statechange(sch, ob, TINO_SCHED_CLEAN);
    case TINO_SCHED_CLEAN:
      tino_sched_statechange(sch, ob, TINO_SCHED_FREE);
      memset(&sch->ob[ob], 0, sizeof sch->ob[ob]);	/* sets state to 0(FREE), too	*/
    case TINO_SCHED_FREE:
      break;
    }
}

/** This only frees the reserved memory.
 *
 * If you must hook the free process, use the callback.
 */
static void
tino_sched_free(tino_scheduler *sch)
{
  int	i;

  if (!sch)
    return;
  sch->q_act	= 0;
  sch->q_inact	= 0;
  tino_free_null(&sch->p);
  for (i=sch->objects; --i>=0; )
    tino_sched_free_ob(sch, i);
      
  000;
}

/** Suspend an old object
 *
 * After a call to this routine, the object can be returned by
 * tino_sched again!
 */
static void
tino_sched_suspend(tino_scheduler *sch, int ob)
{
  000;
}

/** Suspend the old object and fetch the next object from the
 * scheduler.  If there is no old object, use 0 as 'suspend'.
 */
static tino_sched_ob
tino_sched(tino_scheduler *sch, int suspend)
{
  tino_sched_suspend(sch, suspend);
  000;
  return 0;
}

/** This returns a new scheduler object of a certain type.
 *
 * The object is inactive and has precedence and priority 0.
 */
static int
tino_sched_new(tino_scheduler *sch, int type, void *user)
{
  000;
}

static int
tino_sched_set(tino_scheduler *sch, int ob, int prec, int prio)
{
  struct tino_schedob	*o;

  if ((o=tino_schedob(sch, ob))==0)
    return -1;
  o->prec	= prec;
  o->prio	= prio;
  return o;
}

static int
tino_sched_limit(tino_scheduler *sch, int ob, int limit)
{
  struct tino_schedob	*o;

  if ((o=tino_schedob(sch, ob))==0)
    return -1;
  o->limit	= limit;
  return ob;
}

static int
tino_sched_active(tino_scheduler *sch, int ob, int active)
{
  struct tino_schedob	*o;

  if ((o=tino_schedob(sch, ob))==0)
    return -1;
  active	= !!active;
  if (o->active!=active)
    {
      o->active	= active;
      tino_sched_move(sch, o, active ? &ob->sch->q_act : &ob->sch->q_inact);
    }
  return ob;
}

#define	TINO_SCHED_GET(T,X)				\
static T						\
tino_sched_ob_get_##X(tino_scheduler *sch, int ob)	\
{							\
  struct tino_schedob	*o;				\
							\
  return (o=tino_schedob(sch, ob))==0 ? 0 : o->X;	\
}

TINO_SCHED_GET(int,type)
TINO_SCHED_GET(void *,user)
TINO_SCHED_GET(int,active)
TINO_SCHED_GET(int,prec)
TINO_SCHED_GET(int,prio)
TINO_SCHED_GET(int,limit)

#endif
