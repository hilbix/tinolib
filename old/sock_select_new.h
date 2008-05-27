/* $Header$
 *
 * UNIT TEST FAILS *
 *
 * Socket select callback functions
 *
 * This is the new interface, still with poll() methods.
 *
 * Copyright (C)2004-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.1  2008-05-27 21:40:41  tino
 * added (does not work yet)
 *
 */

#ifndef tino_INC_sock_select_h
#define	TINO_SOCK_NEW
#include "sock_select.h"
#else
#ifndef tino_INC_sock_new_h
#define tino_INC_sock_new_h

#define	TINO_SOCK_FREE		(-2)	/* DO NOT USE!	*/
#define	TINO_SOCK_ERR		(-1)	/* routine has an error	(closes)	*/
#define	TINO_SOCK_CLOSE		TINO_SOCK_ERR			/* alias	*/
#define TINO_SOCK_OK		(0)	/* routine is OK (called again)		*/
#define	TINO_SOCK_READ		(1)	/* read blocked or accept done		*/
#define	TINO_SOCK_WRITE		(2)	/* write blocked			*/
#define	TINO_SOCK_EXCEPTION	(4)	/* exception processed			*/

typedef	int tino_sock_proc_fn(TINO_SOCK, int);

static struct tino_sock_imp
  {
    int		n, use;
    TINO_SOCK	list, free;
  } tino_sock_imp;

struct tino_sock
  {
    TINO_SOCK		next, *last;
    int			fd;
    int			state;
    tino_sock_proc_fn	*fn;
    void		*user;
  };

static int
tino_sock_useO(void)
{
  cDP(("() %d", tino_sock_imp.use));
  return tino_sock_imp.use;
}

static inline int
tino_sock_stateO(TINO_SOCK sock)
{
  cDP(("(%p) %d", sock, sock->state)); 
  return sock->state;
}

static inline int
tino_sock_fdO(TINO_SOCK sock)
{
  cDP(("(%p) %d", sock, sock->fd)); 
  return sock->fd;
}

static inline void *
tino_sock_userO(TINO_SOCK sock)
{
  cDP(("(%p) %p", sock, sock->user)); 
  return sock->user;
}

static void
tino_sock_free_impOn(TINO_SOCK sock)
{
  cDP(("(%p)", sock)); 
  sock->process		= 0;
  sock->user		= 0;
  sock->fd		= -1;
  sock->state		= TINO_SOCK_FREE;	/*  nice to have	*/
  sock->next		= tino_sock_imp.free;
  tino_sock_imp.free	= sock;
}

/* Warning, this has a sideeffect:
 *
 * It closes the socket fd if it is open.
 */
static void
tino_sock_freeOns(TINO_SOCK sock)
{
  cDP(("(%p)", sock)); 

  if (sock->fn)
    {
      tmp->state	= TINO_SOCK_CLOSE;
      state		= sock->fn(sock, TINO_SOCK_FREE);
      if (state<0)
	return;
      if (state)
	sock->user	= 0;
    }
  if (sock->user)
    tino_free(sock->user);
  if (sock->fd)
    tino_file_closeE(sock->fd);

  *sock->last	= sock->next;
  if (sock->next)
    sock->next->last	= sock->last;

  tino_sock_imp.use--;
  
  tino_sock_free_impOn(sock);
}

static TINO_SOCK
tino_sock_newAn(tino_sock_proc_fn *fn, void *user)
{
  TINO_SOCK	sock;

  cDP(("(%p,%p)", process, user)); 

  if (!tino_sock_imp.free)
    {
      int	i;
      TINO_SOCK	tmp;

      if (tino_sock_imp.n<64)
      	tino_sock_imp.n	+= 2;
      tmp		=  tino_alloc(tino_sock_imp.n*sizeof *tmp);
      for (i=tino_sock_imp.n; --i>=0; )
	tino_sock_free_impOn(tmp+i);
    }
  sock			= tino_sock_imp.free;
  tino_sock_imp.free	= sock->next;

  tino_sock_imp.use++;

  sock->state		= TINO_SOCK_STATE_IDLE;

  sock->last		= &tino_sock_imp.list;
  sock->next		= tino_sock_imp.list;
  if (sock->next)
    sock->next->last	= &sock->next;
  tino_sock_imp.list	= sock;

  sock->fd		= -1;
  sock->process		= fn;
  sock->user		= user;
  return sock;
}

static TINO_SOCK
tino_sock_new_fdAn(int fd, tino_sock_proc_fn *fn, void *user)
{
  TINO_SOCK	sock;

  cDP(("(%d,%p,%p)", fd, process, user)); 
  if (fd<0)
    tino_sock_error("sock new");
  sock		= tino_sock_newAn(process, user);
  sock->fd	= fd;
  cDP(("() %p", sock)); 
  return sock;
}

/* I know this needs a lot of optimization.
 *
 * By chance rewrite this for libevent.
 *
 * timeout_ms is a default timeout for the timeout structure.  It can
 * be altered using timeout_fn.  This gets the timeout structure on
 * each loop to alter it.  The timeout structure is not passed in case
 * of a timeout (as POSIX does not define the value in this case).
 *
 * Return value:
 * <0	error
 * 0	global EOF (nothing to do)
 * >0	something processed
 */
static int
tino_sock_selectEn(void)
{
  TINO_SOCK		tmp, next;
  TINO_T_fd_set		r, w, e;
  int			loop, n;
  TINO_T_timeval	timeout;
  int			max;

  cDP(("()")); 

#ifdef TINO_ALARM_RUN
  TINO_ALARM_RUN();
#endif

  FD_ZERO(&r);
  FD_ZERO(&w);
  FD_ZERO(&e);
  max	= -1;
  for (tmp=tino_sock_imp.list; tmp; tmp=next)
    {
      int	state;

      next	= tmp->next;
      if (!tmp->fn)
	continue;
      state	= tmp->fn(tmp, tmp->state);
      if (state<=0)
	{
	  if (state<0)
	    {
	      if (tmp->state>=0)
		tmp->state	= TINO_SOCK_CLOSE;
	      continue;
	    }
	  if (tmp->state==TINO_SOCK_CLOSE)
	    {
	      000;
	    }
	  continue;
	}
      if (state&TINO_SOCK_READ)
	FD_SET(tmp->fd, &r);
      if (state&TINO_SOCK_WRITE)
	FD_SET(tmp->fd, &w);
      if (state&TINO_SOCK_EXCEPTION)
	FD_SET(tmp->fd, &e);
      if (tmp->fd>max && state&(TINO_SOCK_READ|TINO_SOCK_WRITE|TINO_SOCK_EXCEPTION))
	max	= tmp->fd;
    }
  cDP(("() select(%d,...)", max+1));
  if (max<0)
    return 0;	/* nothing to do or deadlock	*/
  n	= TINO_F_select(max+1, &r, &w, &e, NULL);
  cDP(("() %d", n));
  if (n<=0)
    {
      if (!n || errno==EINTR)
	return 0;
      return n;
    }
  for (tmp=tino_sock_imp.list; tmp; tmp=next)
    {
      next	= tmp->next;
      if (tmp->fd>=0)
	{
	  int	flag, nothing;

	  cDP(("() check %d", tmp->fd));
	  /* Well, we have a race condition here.
	   * In case we just write something to the socket
	   * and close it immediately (because we only want
	   * to send something) this routine might detect
	   * the EOF on the writing side before it has the
	   * chance to read the data.
	   *
	   * XXX FIXME XXX
	   *
	   * Thus we need to keep the reading side open as long
	   * as it exists.
	   */
	  TINO_XXX;
	  flag	= 1;
	  nothing	= 1;
	  if (FD_ISSET(tmp->fd, &e))
	    {
	      flag	= tmp->process(tmp, TINO_SOCK_EXCEPTION);
	      nothing	= 0;
	    }
	  if (flag>0 && FD_ISSET(tmp->fd, &r))
	    {
	      flag	= tmp->process(tmp, tmp->state&(TINO_SOCK_READ|TINO_SOCK_ACCEPT));
	      nothing	= 0;
	    }
	  if (flag>0 && FD_ISSET(tmp->fd, &w))
	    {
	      flag	= tmp->process(tmp, TINO_SOCK_WRITE);
	      nothing	= 0;
	    }
	  if (nothing)
	    continue;

	  if (flag<0)
	    {
	      if (errno!=EAGAIN && errno!=EINTR)
		tino_sock_freeOns(tmp);
	      continue;
	    }
	  if (!flag)
	    tmp->state	= TINO_SOCK_EOF;
	  tino_sock_pollOn(tmp);
	}
    }
  return n;
}

static int
tino_sock_selectEn(int forcepoll)
{
  return tino_sock_select_timeoutEn(forcepoll, 0l, NULL, NULL);
}

#endif
#endif
