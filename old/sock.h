/* $Header$
 *
 * Socket functions
 * Examples see my tools like ptybuffer/metaproxy/udptx
 *
 * This is far from ready yet.
 * This will sometimes be merged in the successor: io.h
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
 * Revision 1.54  2008-11-03 00:19:51  tino
 * See ChangeLog
 *
 * Revision 1.53  2008-05-19 09:13:38  tino
 * One little comment fixed
 *
 * Revision 1.52  2008-01-03 00:09:37  tino
 * fixes for C++
 *
 * Revision 1.51  2007-09-26 21:29:46  tino
 * make test works again
 *
 * Revision 1.50  2007/09/26 21:09:28  tino
 * Some new functions and Cygwin fixes (started).
 *
 * Revision 1.49  2007/09/21 11:13:20  tino
 * tino_sock_error() calls improved
 *
 * Revision 1.48  2007/09/18 20:07:47  tino
 * Important bugfix in the select_loop.
 * Previously on forced poll strange things could happen.
 * From a starvation to an unconditional return from the loop.
 *
 * Revision 1.47  2007/09/17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.46  2007/08/25 10:28:10  tino
 * Compiles now
 *
 * Revision 1.45  2007/08/25 10:24:52  tino
 * select loop functionality increased
 *
 * Revision 1.44  2007/08/24 10:42:59  tino
 * Function names corrected
 *
 * Revision 1.43  2007/08/19 17:00:38  tino
 * Bugfixes and error handling improved
 *
 * Revision 1.42  2007/08/18 18:57:44  tino
 * tino_sockaddr_t created
 *
 * Revision 1.41  2007/08/17 18:26:21  tino
 * See ChangeLog
 *
 * Revision 1.40  2007/08/08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.39  2007/08/06 02:52:40  tino
 * sock.h (tino_sock_freeOn): now uses tino_file_close
 * Late commit
 *
 * Revision 1.38  2007/05/20 01:02:28  tino
 * Alarm watchdog improved
 *
 * Revision 1.37  2007/05/08 03:15:59  tino
 * tino_sock_shutdownE and partly new naming convention
 *
 * Revision 1.36  2007/04/16 20:51:49  tino
 * Debug flags typos
 *
 * Revision 1.35  2007/04/16 19:52:21  tino
 * See ChangeLog
 *
 * Revision 1.34  2007/04/15 13:44:52  tino
 * Debug calls corrected (oops, never used)
 *
 * Revision 1.33  2007/04/11 14:25:50  tino
 * See Changelog
 *
 * Revision 1.32  2007/03/03 05:21:35  tino
 * More TINO_T_ types
 *
 * Revision 1.31  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 *
 * Revision 1.30  2007/01/25 05:03:16  tino
 * See ChangeLog.  Added functions and improved alarm() handling
 *
 * Revision 1.29  2007/01/22 18:15:16  tino
 * Include fixes
 *
 * Revision 1.28  2006/10/04 02:23:48  tino
 * more tino_va_* routines
 *
 * Revision 1.27  2006/08/22 23:52:15  tino
 * timeout added to tino_sock_select
 *
 * Revision 1.26  2006/07/22 17:24:26  tino
 * See ChangeLog
 *
 * Revision 1.25  2006/02/11 14:36:11  tino
 * 000; is now TINO_XXX;
 *
 * Revision 1.24  2006/01/30 01:17:11  tino
 * created a better note if tino_sock_imp is accessed (which is now disallowed)
 *
 * Revision 1.23  2006/01/29 17:53:35  tino
 * tino_sock_keepalive
 *
 * Revision 1.22  2006/01/29 17:51:35  tino
 * Changes due to strprintf.h
 *
 * Revision 1.21  2006/01/24 22:41:26  tino
 * see changelog (changes for socklinger)
 *
 * Revision 1.20  2006/01/07 18:02:10  tino
 * tino_sockaddr instead of sockaddr_gen and tino_sock_recv added
 *
 * Revision 1.19  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.18  2005/12/03 13:42:15  tino
 * Debugging improved and more helpers
 *
 * Revision 1.17  2005/10/30 03:23:52  tino
 * See ChangeLog
 *
 * Revision 1.16  2005/06/28 20:10:28  tino
 * started to add IOW (IO wrapper)
 *
 * Revision 1.15  2005/01/26 10:53:42  tino
 * Changes due to exception.h
 * Added two functions for code taken from accept.c/socklinger.c
 *
 * Revision 1.14  2004/10/16 21:38:18  tino
 * socket usage count added
 *
 * Revision 1.13  2004/10/10 12:14:53  tino
 * fixes in code which is not yet active
 *
 * Revision 1.12  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.11  2004/07/17 22:23:09  tino
 * started to implement exceptions
 *
 * Revision 1.10  2004/07/17 22:16:21  tino
 * tino_sock_udp started
 *
 * Revision 1.9  2004/06/13 05:39:10  tino
 * bugfix in allocation of more than 16 sockets
 *
 * Revision 1.8  2004/06/13 03:41:52  tino
 * Found a major error in sock.h
 *
 * Revision 1.7  2004/05/23 12:22:22  tino
 * closedown problem in ptybuffer elliminated
 *
 * Revision 1.6  2004/05/21 10:38:06  tino
 * memory corruption solved
 *
 * Revision 1.5  2004/05/20 07:39:28  tino
 * Race condition in case the socket is closed before all data is read.
 * Currently I cannot correct this bug.
 *
 * Revision 1.4  2004/05/19 20:13:23  tino
 * tino_sock_select and associated function added for ptybuffer program
 *
 * Revision 1.3  2004/05/19 05:00:04  tino
 * idea added
 *
 * Revision 1.2  2004/05/01 01:41:06  tino
 * itermediate commit
 *
 * Revision 1.1  2004/04/18 14:18:05  tino
 * added, as I need some standard socket things
 */

#ifndef tino_INC_sock_h
#define tino_INC_sock_h

#include "file.h"
#include "sysfix.h"

#include "fatal.h"
#include "alloc.h"
#include "threads.h"
#include "strprintf.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "syscap.h"

#define	cDP	TINO_DP_sock

/* SORRY!!! This is an awful hack
 *
 * The problem is that I don't use thread safe routines, so there must
 * be a global mutex to prevent accidential reentrancy.
 *
 * Also if one thread registers an error function, another thread
 * cannot do so.  It's not so easy to have several threads register
 * different error functions (yet).
 *
 * (Note that I never use threading myself, as it's not very portable)
 */

TINO_THREAD_SEMAPHORE(tino_sock_sem);

typedef void (*tino_sock_error_fn_t)(TINO_VA_LIST);
static tino_sock_error_fn_t tino_sock_error_fn;

/* Function to ignore (return) socket errors
 */
static void
tino_sock_error_fn_ignore(TINO_VA_LIST list)
{
}

static void
tino_sock_error_lock(tino_sock_error_fn_t fn)
{
  TINO_THREAD_SEMAPHORE_GET(tino_sock_sem);
  tino_sock_error_fn	 = fn;
}

static void
tino_sock_error_unlock(void)
{
  TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
}

/** Socket error processing.
 *
 * Default is to terminate program.  You can override this behavior
 * with an error handler function.  If this routine returns,
 * everything is cleaned up (new!) and the socket function will return
 * some suitable error (like -1 or NULL) if possible.  If not you will
 * not have any indication (except the call to your error handler).
 *
 * This is not completely finished yet ..
 */
static int
tino_sock_error(const char *err, ...)
{
  tino_va_list	list;

  cDP(("(%s,...)", err));
  tino_va_start(list, err);
  if (tino_sock_error_fn)
    tino_sock_error_fn(&list);
  else
    TINO_VEXIT(&list);
  return -1;
}

static int
tino_sock_lingerE(int sock, int ms)
{
  struct linger		linger;

  cDP(("(%d,%d)", sock, ms));
  if (ms>=0)
    {
      linger.l_onoff        = 1;
      linger.l_linger       = ms;
    }
  else
    {
      linger.l_onoff        = 0;
      linger.l_linger       = 0;
    }
  return TINO_F_setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof linger);
}

static void
tino_sock_linger(int sock, int linger)
{
  if (tino_sock_lingerE(sock, linger))
    tino_sock_error("tino_sock_linger");
}

static int
tino_sock_keepaliveE(int sock, int on)
{
  cDP(("(%d,%d)", sock, on));
  return TINO_F_setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);
}

static void
tino_sock_keepalive(int sock, int on)
{
  if (tino_sock_keepaliveE(sock, on))
    tino_sock_error("tino_sock_keepalive");
}

static int
tino_sock_reuseE(int sock, int on)
{
  cDP(("(%d,%d)", sock, on));
  return TINO_F_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
}

static void
tino_sock_reuse(int sock, int on)
{
  if (tino_sock_reuseE(sock, on))
    tino_sock_error("tino_sock_reuse");
}

static int
tino_sock_rcvbufE(int sock, int size)
{
  cDP(("(%d,%d)", sock, size));
  return TINO_F_setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof size);
}

static void
tino_sock_rcvbuf(int sock, int size)
{
  if (tino_sock_rcvbufE(sock, size))
    tino_sock_error("tino_sock_rcvbuf");
}

static int
tino_sock_sndbufE(int sock, int size)
{
  cDP(("(%d,%d)", sock, size));
  return TINO_F_setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof size);
}

static void
tino_sock_sndbuf(int sock, int size)
{
  if (tino_sock_sndbufE(sock, size))
    tino_sock_error("tino_sock_sndbuf");
}


#if 1
/* INTERMEDIATE TOOLS.
 * THIS WILL BE REORGANIZED TO A COMMON API
 * However for now I don't have the time to do it right,
 * so just do it quick and dirty.
 */

/* Following once was once taken out of my tool accept-2.0.0 with some
 * lines deleted, now it's mostly rewritten
 */
typedef struct tino_sockaddr
   {
     socklen_t			len;	/* 0 if not initialized or error	*/
     union
       {
	 TINO_T_sockaddr	sa;
	 TINO_T_sockaddr_un	un;
	 TINO_T_sockaddr_in	in;
#ifdef	TINO_HAS_IPv6
	 TINO_T_sockaddr_in6	in6;
#endif
       }			sa;
   } tino_sockaddr_t;

/** Translate a named string into a socket address.
 *
 * This knows of Unix domain sockets, IPv4 and (if defined) IPv6
 *
 * This currently is not really thread ready!
 *
 * The last ':' separates the name from the port.  This is true even
 * for IPv6.  If there is no ':' present, it's taken as a path for an
 * unix domain socket.
 */
static int
tino_sock_getaddr(tino_sockaddr_t *sin, const char *adr)
{
  char		*s, *host;
  int		max;
  size_t	len;

  cDP(("(%p,%s)", sin, adr));

  len	= strlen(adr)+1;
  host	= (char *)TINO_F_alloca(len);
  memcpy(host, adr, len);

  memset(sin, 0, sizeof *sin);

  s	= strrchr(host, ':');
  if (s)
    {
      *s	= 0;

      sin->sa.in.sin_family	= AF_INET;
      sin->sa.in.sin_addr.s_addr= TINO_F_htonl(INADDR_ANY);
      sin->sa.in.sin_port	= TINO_F_htons(atoi(s+1));
      sin->len			= sizeof sin->sa.in;

      if (s!=host)
	{
	  struct hostent	*he;

	  TINO_THREAD_SEMAPHORE_GET(tino_sock_sem);
	  he	= TINO_F_gethostbyname(host);
	  if (!he)
	    {
	      TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
	      sin->len	= 0;
	      return tino_sock_error("cannot resolve %s", host);
	    }
#ifdef	TINO_HAS_IPv6
	  if (he->h_addrtype==AF_INET6 && he->h_length>=(int)sizeof sin->sa.in6.sin6_addr)
	    {
	      sin->sa.in6.sin6_family	= AF_INET6;
	      sin->sa.in6.sin6_port	= TINO_F_htons(atoi(s+1));
	      sin->len			= sizeof sin->sa.in6;
	      memcpy(&sin->sa.in6.sin6_addr, he->h_addr, sizeof sin->sa.in6.sin6_addr);
	      TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
	      return 0;
	    }
#endif
	  if (he->h_addrtype!=AF_INET || he->h_length!=sizeof sin->sa.in.sin_addr)
	    {
	      int	addrtype	= he->h_addrtype;

	      TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
	      sin->len	= 0;
	      return tino_sock_error("unsupported host address type: %d, must be %d(AF_INET)", addrtype, AF_INET);
	    }
	  memcpy(&sin->sa.in.sin_addr, he->h_addr, sizeof sin->sa.in.sin_addr);
	  TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
	}
      return 0;
    }

  sin->sa.un.sun_family	= AF_UNIX;

  max = strlen(host);
  if (max >= (int)sizeof(sin->sa.un.sun_path))
    {
      sin->len	= 0;
      return tino_sock_error("path too long: %s", host);
    }
  strncpy(sin->sa.un.sun_path, host, max);
  sin->len	= max + sizeof sin->sa.un.sun_family;
  return 0;
}

/** Warning!  If you catch errors, be sure to test for calls to the
 * error handler after a connect.  This routine can succeed even if
 * some transient nonfatal errors happened (like a proper bind could
 * not be done).  You see the error through the error handler, so be
 * sure to properly handle it!
 */
static int
tino_sock_tcp_connect(const char *to, const char *local)
{
  tino_sockaddr_t	sa, l_sa;
  int			sock;

  if (tino_sock_getaddr(&sa, to))
    return -1;

  l_sa.len	= 0;	/* Suppress warning	*/
  if (local)
    {
      if (tino_sock_getaddr(&l_sa, local))
	return -1;
#if 0
      if (l_sa.sa.sa_family!=sa.sa.sa_family)
	return tino_sock_error("local and remote protocol do not match");
#endif
    }

  sock	= TINO_F_socket(sa.sa.sa.sa_family, SOCK_STREAM, 0);
  if (sock<0)
    return tino_sock_error("socket");

  tino_sock_reuse(sock, 1);

  /* Beeing unable to bind is a transient nonfatal error!
   *
   * This might be a security problem if the interface you want to
   * bind to goes away and thus the default interface is used instead.
   */
  if (local && TINO_F_bind(sock, &l_sa.sa.sa, l_sa.len))
    tino_sock_error("bind");

  if (TINO_F_connect(sock, &sa.sa.sa, sa.len))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("connect");
    }

  return sock;
}

static int
tino_sock_tcp_listen(const char *s)
{
  tino_sockaddr_t	sin;
  int			sock;

  if (tino_sock_getaddr(&sin, s)<0)
    return -1;

  sock	= TINO_F_socket(sin.sa.sa.sa_family, SOCK_STREAM, 0);
  if (sock<0)
    return tino_sock_error("socket");

  /* Set reuse to true.
   * The idea about this is,
   * that binding to an address shall not fail,
   * in case that the old service has not terminated yet.
   */
  tino_sock_reuse(sock, 1);

  if (TINO_F_bind(sock, &sin.sa.sa, sin.len))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("bind");
    }

  if (TINO_F_listen(sock, 100))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("listen");
    }

  return sock;
}
/* END COPY
 */

/** Create and bind UDP socket.
 */
static int
tino_sock_udp_sa(const char *src, tino_sockaddr_t *sin)
{
  int	sock;

  if (tino_sock_getaddr(sin, src))
    return -1;
  sock	= TINO_F_socket(sin->sa.sa.sa_family, SOCK_DGRAM, 0);
  if (sock<0)
    return tino_sock_error("tino_sock_udp(socket)");

  /* Reusing UDP sockets generally is a bad idea in this respect
   */
  tino_sock_reuse(sock, 0);

  tino_sock_rcvbuf(sock, 100000);
  tino_sock_sndbuf(sock, 100000);

  /* If bind does not work, the socket is most times unusable.  It
   * still can be used for 'connect', but not for sendto().
   */
  if (TINO_F_bind(sock, &sin->sa.sa, sin->len))
    tino_sock_error("tino_sock_udp(bind)");

  return sock;
}

static int
tino_sock_udp(const char *src)
{
  tino_sockaddr_t	sin;

  return tino_sock_udp_sa(src, &sin);
}

#endif

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
#if 0
struct tino_sock_addr_gen
  {
    union
      {
	TINO_T_sockaddr		sa;
	TINO_T_sockaddr_un	un;
	TINO_T_sockaddr_in	in;
      }				addr;		/* family, address	*/
    int				type, proto;	/* type and protocol	*/
  };


static enum tino_sock_type
  {
    TINO_SOCK_AUTO		=0,
    TINO_SOCK_UNIX_STREAM	=1,
    TINO_SOCK_UNIX_DGRAM	=2,
    TINO_SOCK_TCP		=3,
    TINO_SOCK_UDP		=4,
  };

/* NOT READY YET
 *
 * Open a socket of given type.
 * If type is AUTO this function tries to autodetects the type of socket.
 *
 * Autodetection is done as follows:
 * "unix:"	Unix-type stream socket
 * "dgram:"	Unix-type dgram socket
 * "tcp:"	TCP-type socket (stream)
 * "udp:"	UDP-type socket (dgram)
 */
static int
tino_sock_getaddr(union tino_sockaddr *sin, int type, const char *adr)
{
  int						type2;
  static struct { const char *s; int type; }	types[] =
    {
      { "unix:",	TINO_SOCK_UNIX	},
      { "dgram:",	TINO_SOCK_DGRAM	},
      { "tcp:",		TINO_SOCK_TCP	},
      { "udp:",		TINO_SOCK_UDP	},
    };

  type2	= TINO_SOCK_ANY;
  if (adr)
    {
      int	i;

      for (i=sizeof types/sizeof *types; --i>=0; i++)
	{
	  const char	*p;
	  if ((p=tino_prefixcmp(adr, types[i].s))!=0)
	    {
	      type2	= types[i].type;
	      adr	= p;
	      break;
	    }
	}
    }
  if (type==TINO_SOCK_AUTO)
    type	= type2;
  if (type==TINO_SOCK_AUTO)
    {
      000;
    }
  if (type2!=TINO_SOCK_AUTO && type!=type2)
    tino_fatal("socket type invalid: %d", type2);
  switch (type)
    {
    case TINO_SOCK_STREAM:
    case TINO_SOCK_UNIX_DGRAM
    case TINO_SOCK_TCP:
    case TINO_SOCK_UDP:
    }

  char		*s, *host;
  int		max;
  size_t	len;

  len	= strlen(adr)+1;
  host	= TINO_F_alloca(len);
  memcpy(host, adr, len);

  memset(sin, 0, sizeof *sin);
  for (s=host; *s; s++)
    if (*s==':')
      {
	*s	= 0;

	sin->sa.in.sin_family	= AF_INET;
	sin->sa.in.sin_addr.s_addr	= TINO_F_htonl(INADDR_ANY);
	sin->sa.in.sin_port	= TINO_F_htons(atoi(s+1));

	if (s!=host && !TINO_F_inet_aton(host, &sin->sa.in.sin_addr))
	  {
#ifdef	TINO_SOCK_NO_RESOLVE
	    tino_sock_error("%s", host));
#else
	    struct hostent	*he;

	    if ((he=TINO_F_gethostbyname(host))==0)
	      tino_sock_error("%s", host);
	    if (he->h_addrtype!=AF_INET || he->h_length!=sizeof sin->sa.in.sin_addr
	      tino_sock_error("unsupported host address");
	    memcpy(&sin->sa.in.sin_addr, he->h_addr, sizeof sin->sa.in.sin_addr);
#endif
	  }
	return sizeof *sin;
      }

  sin->sa.un.sun_family	= AF_UNIX;

  max = strlen(host);
  if (max > sizeof(sin->sa.un.sun_path)-1)
    max = sizeof(sin->sa.un.sun_path)-1;
  strncpy(sin->sa.un.sun_path, host, max);

  return max + sizeof sin->sa.un.sun_family;
}

static int
tino_sock_udp(const char *name, int do_listen)
{
  struct tino_sock_addr_gen	sa;
  int				sock;
  int				on;

  sock	= TINO_F_socket(AF_INET, SOCK_DGRAM, 0);

  on	= 102400;
  if (TINO_F_setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &on, sizeof on))
    tino_sock_error("setsockopt sndbuf");

  on	= 102400;
  if (TINO_F_setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &on, sizeof on))
    tino_sock_error("setsockopt rcvbuf");

  tino_sock_getaddr(&sa, TINO_SOCK_UDP, name);

  if (TINO_F_bind(sock, (TINO_T_sockaddr *)&sa.addr.in, sizeof sa.addr.in))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("bind");
    }
  return sock;
}
#endif
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static int
tino_sock_unix(const char *name, int do_listen)
{
  TINO_T_sockaddr_un	sun;
  int			sock;
  int			max;

  sun.sun_family    = AF_UNIX;

  max = strlen(name);
  if (max > (int)sizeof(sun.sun_path)-1)
    max = sizeof(sun.sun_path)-1;
  strncpy(sun.sun_path, name, max);
  sun.sun_path[max]	= 0;

  max += sizeof sun.sun_family;

  sock	= TINO_F_socket(sun.sun_family, SOCK_STREAM, 0);
  if (sock<0)
    return tino_sock_error("socket");

  if (do_listen>0)
    {
      umask(0);
      if (TINO_F_bind(sock, (TINO_T_sockaddr *)&sun, max+sizeof sun.sun_family))
	{
	  tino_file_close_ignO(sock);
	  return tino_sock_error("bind");
	}
      if (TINO_F_listen(sock, do_listen))
	{
	  tino_file_close_ignO(sock);
	  return tino_sock_error("listen");
	}
    }
  else if (TINO_F_connect(sock, (TINO_T_sockaddr *)&sun, max+sizeof sun.sun_family))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("connect");
    }
  return sock;
}

static int
tino_sock_unix_connect(const char *name)
{
  return tino_sock_unix(name, 0);
}

static int
tino_sock_unix_listen(const char *name)
{
  return tino_sock_unix(name, 1);
}

static char *
tino_sock_get_adrnameN(tino_sockaddr_t *sa)
{
  char	buf[256];

  switch (sa->sa.sa.sa_family)
    {
    case AF_UNIX:
      TINO_XXX;	/* well, can we find out something?	*/
      return tino_str_printf("(unix:%s)", sa->sa.un.sun_path);

#ifdef	TINO_HAS_IPv6
    case AF_INET6:
      if (!TINO_F_inet_ntop(sa->sa.sa.sa_family, &sa->sa.in6.sin6_addr, buf, sizeof buf))
	return 0;
      return tino_str_printf("[%s]:%ld", buf, TINO_F_ntohs(sa->sa.in6.sin6_port));
#endif
    case AF_INET:
      if (!TINO_F_inet_ntop(sa->sa.sa.sa_family, &sa->sa.in.sin_addr, buf, sizeof buf))
	return 0;
      return tino_str_printf("%s:%ld", buf, TINO_F_ntohs(sa->sa.in.sin_port));
    }
  return 0;
}

/* you must free the return value
 */
static char *
tino_sock_get_peernameN(int fd)
{
  tino_sockaddr_t	sa;

  if (fd<0)
    return 0;
  sa.len	= sizeof sa.sa;
  if (TINO_F_getpeername(fd, &sa.sa.sa, &sa.len))
    return 0;
  return tino_sock_get_adrnameN(&sa);
}

/* You must free the return value
 */
static char *
tino_sock_get_socknameN(int fd)
{
  tino_sockaddr_t	sa;

  sa.len	= sizeof sa;
  if (TINO_F_getsockname(fd, &sa.sa.sa, &sa.len))
    return 0;
  return tino_sock_get_adrnameN(&sa);
}

static int
tino_sock_recvI(int fd, void *buf, size_t len, tino_sockaddr_t *adr)
{
  int		got;

  adr->len	= sizeof adr->sa;
  got		= TINO_F_recvfrom(fd, buf, len, 0, (adr ? &adr->sa.sa : NULL), &adr->len);
  return got;
}

/** Create a socketpair with standard parameters.
 */
static void
tino_sock_pairA(int socks[2])
{
  if (TINO_F_socketpair(AF_UNIX, SOCK_STREAM, 0, socks))
    {
      tino_sock_error("socketpair");
      socks[0]	= -1;
      socks[1]	= -1;
    }
}

static int
tino_sock_accept_addrI(int fd, tino_sockaddr_t *sa)
{
  return TINO_F_accept(fd, sa ? &sa->sa.sa : NULL, sa ? &sa->len : NULL);
}

static int
tino_sock_acceptI(int fd)
{
  return tino_sock_accept_addrI(fd, NULL);
}

static int
tino_sock_shutdownE(int fd, int side)
{
  return TINO_F_shutdown(fd, side);
}

#undef	cDP
#endif
