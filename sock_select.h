/* Socket select callback functions
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_sock_select_h
#define tino_INC_sock_select_h

#include "sock.h"

#include <sys/select.h>	/* remove this if your system does not have it	*/

#define	cDP	TINO_DP_sock

/* This will be changed unconditionally from time to time.  Never even
 * try to access it directly!
 */
#define	tino_sock_imp	tino_sock_imp_0283838kf9w89r2	/*TiNoAuToRoT*/

/* You shall not assume anything about this type!
 * Later its implementation will change.
 * All you know is:
 * It's a pointer which never changes as long as this socket exists.
 *
 * Access it via the dedicated functions, only!
 */
typedef struct tino_sock *TINO_SOCK;


#ifndef TINO_SOCK_NEW
#include "sock_select_old.h"
#else
#include "sock_select_new.h"
#endif


/* This is a generic interface!
 *
 * Binding to a local address will (must) be implemented using
 * prefixes.
 */
static TINO_SOCK
tino_sock_new_connect(const char *target,
		      int (*process)(TINO_SOCK, enum tino_sock_proctype),
		      void *user)
{
  cDP(("('%s',%p,%p)", target, process, user)); 
  return tino_sock_new_fdANn(tino_sock_tcp_connect(target, NULL), process, user);
}

static TINO_SOCK
tino_sock_new_listen(const char *bind,
		     int (*process)(TINO_SOCK, enum tino_sock_proctype),
		     void *user)
{
  cDP(("('%s',%p,%p)", bind, process, user)); 
  return tino_sock_new_fdANn(tino_sock_tcp_listen(bind), process, user);
}


/* Do the standard looping.
 *
 * There is a checkfunc which is checked on each loop.  If this
 * function returns -1 then the loop is interrupted.  If this routine
 * returns >0 a forced poll is done.
 *
 * returns:
 * <0 if checkfunc() returns <0
 * 0 if ok
 * 1 on error (if error is ignored)
 *
 * timout_fn is the timeout function from the select loop.  The user
 * pointer is shared between all calls.
 */
static int
tino_sock_select_loop3A(int (*checkfunc)(void *), void (*timeout_fn)(TINO_T_timeval *, void *), void *user)
{
  int	tmp;

  cDP(("(%p,%p)", checkfunc, user));
  do
    {
      tmp	= 0;
      if (checkfunc)
	{
	  cDP(("() checkfunc[%p](%p)", checkfunc, user));
	  if ((tmp=checkfunc(user))<0)
	    {
	      cDP(("() %d", tmp));
	      return tmp;
	    }
	}
    } while ((tmp=tino_sock_select_timeoutEn(tmp, 0l, timeout_fn, user))>0);

  if (tmp<0)
    {
      tino_sock_error("tino_sock_select_loop select() error");
      cDP(("() err"));
      return 1;
    }
  cDP(("() ok"));
  return 0;
}

static int
tino_sock_select_loop2A(void (*timeout_fn)(TINO_T_timeval *, void *), void *user)
{
  return tino_sock_select_loop3A(NULL, timeout_fn, user);
}

static int
tino_sock_select_loop1A(int (*checkfunc)(void *), void *user)
{
  return tino_sock_select_loop3A(checkfunc, NULL, user);
}

static int
tino_sock_select_loopA(void)
{
  return tino_sock_select_loop3A(NULL, NULL, NULL);
}

#undef tino_sock_imp
#define tino_sock_imp	$tino_sock_imp_cannot_be_accessed$

#undef	cDP
#endif
