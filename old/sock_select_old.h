/* $Header$
 *
 * Socket select callback functions
 *
 * This is the old interface with poll() methods.
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
 * Revision 1.1  2007-09-26 21:09:28  tino
 * Some new functions and Cygwin fixes (started).
 *
 */

#ifndef tino_INC_sock_old_h
#define tino_INC_sock_old_h

#ifndef tino_INC_sock_select_h
#error	"only include from tino/sock_select.h"
#endif

/* Type of the processing function:
 * If the function returns <0 the socket will enter state TINO_SOCK_ERR
 * If the function returns 0 on READ we have EOF on READ.
 * If the function returns 0 on WRITE we have EOF on WRITE.
 *
 * CLOSE:
 * The socket will be invalidated afterwards, called if POLL or EOF return <0
 * POLL and EOF are similar, they must return the new tino_sock_state.
 *
 * If the function is not defined, and user!=0 then the user-pointer
 * is destroyed on tino_sock_free, This means free() or tino_ob_destroy().
 */

enum tino_sock_numbers
  {
    TINO_SOCK_FREE	= -3,	/* free(user) on close	*/
    TINO_SOCK_ERR	= -2,
    TINO_SOCK_CLOSE	= TINO_SOCK_ERR,
    TINO_SOCK_EOF	= -1,
    TINO_SOCK_POLL	= 0,
    TINO_SOCK_READ	= 1,
    TINO_SOCK_WRITE	= 2,
    TINO_SOCK_READWRITE	= TINO_SOCK_READ+TINO_SOCK_WRITE,
    TINO_SOCK_EXCEPTION	= 4,
    TINO_SOCK_ACCEPT	= 8,
  };

enum tino_sock_proctype
  {
    TINO_SOCK_PROC_CLOSE	= TINO_SOCK_CLOSE,	/* close. free(user)! */
    TINO_SOCK_PROC_EOF		= TINO_SOCK_EOF,	/* EOF encountered, */
    TINO_SOCK_PROC_POLL		= TINO_SOCK_POLL,	/* return bitmask: */
    TINO_SOCK_PROC_READ		= TINO_SOCK_READ,
    TINO_SOCK_PROC_WRITE	= TINO_SOCK_WRITE,
    TINO_SOCK_PROC_EXCEPTION	= TINO_SOCK_EXCEPTION,
    TINO_SOCK_PROC_ACCEPT	= TINO_SOCK_ACCEPT,
  };

enum tino_sock_state
  {
    TINO_SOCK_STATE_FREE	= TINO_SOCK_FREE,
    TINO_SOCK_STATE_ERR		= TINO_SOCK_ERR,
    TINO_SOCK_STATE_EOF		= TINO_SOCK_EOF,
    TINO_SOCK_STATE_IDLE	= TINO_SOCK_POLL,
    TINO_SOCK_STATE_READ	= TINO_SOCK_READ,	/* read() needed */
    TINO_SOCK_STATE_WRITE	= TINO_SOCK_WRITE,	/* write() needed */
    TINO_SOCK_STATE_READWRITE	= TINO_SOCK_READWRITE,	/* both needed */
    TINO_SOCK_STATE_ACCEPT	= TINO_SOCK_ACCEPT,	/* accept() needed */
  };

#if 0
enum tino_sock_flags
  {
    TINO_SOCK_FLAGS_READ	= 1,	/* read() supported	*/
    TINO_SOCK_FLAGS_WRITE	= 2,	/* write() supported	*/
    TINO_SOCK_FLAGS_EXCEPTION	= 4,	/* exception (OOB data) supported */
    TINO_SOCK_FLAGS_ACCEPT	= 8,	/* accept() supported	*/
  };
#endif

static struct tino_sock_imp
  {
    int		n, use, forcepoll;
    TINO_SOCK	list, free;
  } tino_sock_imp;

struct tino_sock
  {
    TINO_SOCK		next, *last;
    int			state;
#if 0
    int			flags;
#endif
    int			fd;
    int			(*process)(TINO_SOCK, enum tino_sock_proctype);
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

#if 0
static inline int
tino_sock_flagsO(TINO_SOCK sock)
{
  cDP(("(%p) %d", sock, sock->flags)); 
  return sock->flags;
}
#endif

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
  sock->state		= TINO_SOCK_STATE_FREE;	/*  nice to have	*/

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

  if (!sock->process || sock->process(sock, TINO_SOCK_CLOSE)==TINO_SOCK_FREE)
    {
      if (sock->user)
	tino_free(sock->user);
    }
  if (sock->fd)
    tino_file_closeE(sock->fd);

  *sock->last	= sock->next;
  if (sock->next)
    sock->next->last	= sock->last;

  tino_sock_imp.use--;
  
  tino_sock_free_impOn(sock);
}

static TINO_SOCK
tino_sock_newAn(int (*process)(TINO_SOCK, enum tino_sock_proctype),
		void *user)
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
  tino_sock_imp.forcepoll	= 1;

  sock->state		= TINO_SOCK_STATE_IDLE;
#if 0
  sock->flags		= 0;
#endif

  sock->last		= &tino_sock_imp.list;
  sock->next		= tino_sock_imp.list;
  if (sock->next)
    sock->next->last	= &sock->next;
  tino_sock_imp.list	= sock;

  sock->fd		= -1;
  sock->process		= process;
  sock->user		= user;
  return sock;
}

static TINO_SOCK
tino_sock_new_fdAn(int fd,
		   int (*process)(TINO_SOCK, enum tino_sock_proctype),
		   void *user)
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

/* You *must* call this:
 * - after the socket has been created
 * - if the state of a socket may have changed
 * If you don't do it, your program will not work.
 * Hint: Use the forcepoll==1 on tino_sock_select to get all sockets polled.
 */
static TINO_SOCK
tino_sock_pollOn(TINO_SOCK sock)
{
  int	state;

  cDP(("(%p) state=%d", sock, sock->state)); 
  state	= sock->process(sock, sock->state<0 ? TINO_SOCK_EOF : TINO_SOCK_POLL);
  sock->state	= state;
  if (state<0)
    tino_sock_freeOns(sock);
  cDP(("() state=%d", state)); 
  return sock;
}

static void
tino_sock_forcepollO(void)
{
  cDP(("()")); 
  tino_sock_imp.forcepoll	= 1;
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
 * 0	global EOF (nothing to do or timeout without timeout_fn)
 * >0	something processed
 */
static int
tino_sock_select_timeoutEn(int forcepoll, long timeout_ms, void (*timeout_fn)(TINO_T_timeval *, void *), void *user)
{
  TINO_SOCK		tmp, next;
  TINO_T_fd_set		r, w, e;
  int			loop, n;
  TINO_T_timeval	timeout;

  cDP(("(%d, %ld, %p, %p)", forcepoll, timeout_ms, timeout_fn, user)); 
  for (loop=0;; loop++)
    {
      int	max;

#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif

      if (tino_sock_imp.forcepoll)
	forcepoll	= 1;
      tino_sock_imp.forcepoll	= 0;

      FD_ZERO(&r);
      FD_ZERO(&w);
      FD_ZERO(&e);
      max	= -1;
      for (tmp=tino_sock_imp.list; tmp; tmp=next)
	{
	  next	= tmp->next;
	  if (tmp->fd<0)
	    continue;
	  if (forcepoll)
	    tino_sock_pollOn(tmp);
	  if (tmp->state>0)
	    {
	      if (tmp->state&(TINO_SOCK_READ|TINO_SOCK_ACCEPT))
		FD_SET(tmp->fd, &r);
	      if (tmp->state&TINO_SOCK_WRITE)
		FD_SET(tmp->fd, &w);
	      if (tmp->state&TINO_SOCK_EXCEPTION)
		FD_SET(tmp->fd, &e);
	      if (tmp->fd>max)
		max	= tmp->fd;
	    }
	}
      timeout.tv_sec	= timeout_ms/1000;
      timeout.tv_usec	= (timeout_ms%1000)*1000;
      if (timeout_fn)
	timeout_fn(&timeout, user);

      cDP(("() select(%d,...)", max+1));
      if (max<0 && !timeout.tv_sec && !timeout.tv_usec)
	return 0;
      if ((n=TINO_F_select(max+1, &r, &w, &e, ((timeout.tv_sec || timeout.tv_usec) ? &timeout : NULL)))>0)
	break;
      cDP(("() %d", n));
      if (!n)
	{
	  /* Timeout
	   */
	  if (timeout_fn)
	    timeout_fn(NULL, user);
	  else
	    return n;
	}
      if (errno!=EINTR && (errno!=EAGAIN || loop>1000))
	return n;
    }
  cDP(("() %d", n));
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
