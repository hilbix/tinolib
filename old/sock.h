/* $Header$
 *
 * Socket helpers
 * Examples see my tools like ptybuffer/metaproxy/udptx
 *
 * This is far from ready yet.
 * This will sometimes be merged in the successor: io.h
 *
 * Copyright (C)2004-2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.30  2007-01-25 05:03:16  tino
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

#include "fatal.h"
#include "alloc.h"
#include "threads.h"
#include "strprintf.h"

#include <sys/select.h>	/* remove this if your system does not have it	*/
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define	cDP	TINO_DP_sock

/* You shall not assume anything about this type!
 * Later its implementation will change.
 * All you know is:
 * It's a pointer which never changes as long as this socket exists.
 *
 * Access it via the dedicated functions, only!
 */
typedef struct tino_sock *TINO_SOCK;

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

typedef void (*tino_sock_error_fn_t)(const char *err, TINO_VA_LIST list);
static tino_sock_error_fn_t tino_sock_error_fn;

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

static void
tino_sock_error(const char *err, ...)
{
  tino_va_list	list;

  tino_va_start(list, err);
  if (tino_sock_error_fn)
    tino_sock_error_fn(err, &list);
  else
    TINO_VEXIT(err, &list);
  tino_fatal("tino_sock_error returns");
}


static int
tino_sock_linger_err(int sock, int ms)
{
  struct linger		linger;

  DP(("tino_sock_linger_err(%s,%d)", sock, ms));
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
  return setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof linger);
}

static void
tino_sock_linger(int sock, int linger)
{
  if (tino_sock_linger_err(sock, linger))
    tino_sock_error("tino_sock_linger");
}

static int
tino_sock_keepalive_err(int sock, int on)
{
  DP(("tino_sock_keepalive_err(%s,%d)", sock, on));
  return setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);
}

static void
tino_sock_keepalive(int sock, int on)
{
  if (tino_sock_keepalive_err(sock, on))
    tino_sock_error("tino_sock_keepalive");
}

static int
tino_sock_reuse_err(int sock, int on)
{
  DP(("tino_sock_reuse_err(%s,%d)", sock, on));
  return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
}

static void
tino_sock_reuse(int sock, int on)
{
  if (tino_sock_reuse_err(sock, on))
    tino_sock_error("tino_sock_reuse");
}

static int
tino_sock_rcvbuf_err(int sock, int size)
{
  DP(("tino_sock_rcvbuf_err(%s,%d)", sock, size));
  return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof size);
}

static void
tino_sock_rcvbuf(int sock, int size)
{
  if (tino_sock_rcvbuf_err(sock, size))
    tino_sock_error("tino_sock_rcvbuf");
}

static int
tino_sock_sndbuf_err(int sock, int size)
{
  DP(("tino_sock_sndbuf_err(%s,%d)", sock, size));
  return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof size);
}

static void
tino_sock_sndbuf(int sock, int size)
{
  if (tino_sock_sndbuf_err(sock, size))
    tino_sock_error("tino_sock_sndbuf");
}


#if 1
/* INTERMEDIATE TOOLS.
 * THIS WILL BE REORGANIZED TO A COMMON API
 * However for now I don't have the time to do it right,
 * so just do it quick and dirty.
 */

/* Following was taken out of my tool
 * accept-2.0.0
 * with some lines deleted
 */
union tino_sockaddr
   {
     struct sockaddr		sa;
     struct sockaddr_un		un;
     struct sockaddr_in		in;
#ifdef IPPROTO_IPV6
     struct sockaddr_in6	in6;
#endif
  };

static int
tino_sock_getaddr(union tino_sockaddr *sin, const char *adr)
{
  char		*s, *host;
  int		max;
  size_t	len;

  len	= strlen(adr)+1;
  host	= alloca(len);
  memcpy(host, adr, len);

  memset(sin, 0, sizeof *sin);

  s	= strrchr(host, ':');
  if (s)
    {
      *s	= 0;

      sin->in.sin_family	= AF_INET;
      sin->in.sin_addr.s_addr	= htonl(INADDR_ANY);
      sin->in.sin_port		= htons(atoi(s+1));

      if (s!=host)
	{
	  struct hostent	*he;

	  TINO_THREAD_SEMAPHORE_GET(tino_sock_sem);
	  he	= gethostbyname(host);
	  TINO_THREAD_SEMAPHORE_FREE(tino_sock_sem);
	  if (!he)
	    tino_sock_error("cannot resolve %s", host);
#ifdef IPPROTO_IPV6
	  if (he->h_addrtype==AF_INET6 && he->h_length==sizeof sin->in6.sin6_addr)
	    {
	      sin->in6.sin6_family	= AF_INET6;
	      sin->in6.sin6_port	= htons(atoi(s+1));
	      memcpy(&sin->in6.sin6_addr, he->h_addr, sizeof sin->in6.sin6_addr);
	      return sizeof *sin;
	    }
#endif
	  if (he->h_addrtype!=AF_INET || he->h_length!=sizeof sin->in.sin_addr)
	    tino_sock_error("unsupported host address type: %d, must be %d(AF_INET)", he->h_addrtype, AF_INET);
	  memcpy(&sin->in.sin_addr, he->h_addr, sizeof sin->in.sin_addr);
	}
      return sizeof *sin;
    }

  sin->un.sun_family	= AF_UNIX;

  max = strlen(host);
  if (max >= sizeof(sin->un.sun_path))
    tino_sock_error("path too long: %s", host);
  strncpy(sin->un.sun_path, host, max);

  return max + sizeof sin->un.sun_family;
}

static int
tino_sock_tcp_connect(const char *to, const char *local)
{
  union tino_sockaddr	sa;
  int			len;
  int			sock;

  len	= tino_sock_getaddr(&sa, to);

  sock	= socket(sa.sa.sa_family, SOCK_STREAM, 0);
  if (sock<0)
    tino_sock_error("socket");

  tino_sock_reuse(sock, 1);

  if (local)
    {
      union tino_sockaddr	l_sa;
      int			l_len;

      l_len	= tino_sock_getaddr(&l_sa, local);

#if 0
      if (l_sa.sa.sa_family!=sa.sa.sa_family)
	tino_sock_error("local and remote protocol do not match");
#endif
      if (bind(sock, &l_sa.sa, l_len))
	tino_sock_error("bind");
    }
  if (connect(sock, &sa.sa, len))
    tino_sock_error("connect");

  return sock;
}

static int
tino_sock_tcp_listen(const char *s)
{
  union tino_sockaddr	sin;
  int			len;
  int			sock;

  len	= tino_sock_getaddr(&sin, s);

  sock	= socket(sin.sa.sa_family, SOCK_STREAM, 0);
  if (sock<0)
    tino_sock_error("socket");

  /* Set reuse to true.
   * The idea about this is,
   * that binding to an address shall not fail,
   * in case that the old service has not terminated yet.
   */
  tino_sock_reuse(sock, 1);

  if (bind(sock, &sin.sa, len))
    tino_sock_error("bind");

  if (listen(sock, 100))
    tino_sock_error("listen");

  return sock;
}
/* END COPY
 */

/** Create and bind UDP socket.
 */
static int
tino_sock_udp(const char *src)
{
  union tino_sockaddr	sin;
  int			len;
  int			sock;

  len	= tino_sock_getaddr(&sin, src);
  sock	= socket(sin.sa.sa_family, SOCK_DGRAM, 0);
  if (sock<0)
    tino_sock_error("tino_sock_udp(socket)");

  /* Reusing UDP sockets generally is a bad idea in this respect
   */
  tino_sock_reuse(sock, 0);

  tino_sock_rcvbuf(sock, 100000);
  tino_sock_sndbuf(sock, 100000);

  if (bind(sock, &sin.sa, len))
    tino_sock_error("tino_sock_udp(bind)");

  return sock;
}

#endif

#if 0
struct tino_sock_addr_gen
  {
    union
      {
	struct sockaddr		sa;
	struct sockaddr_un	un;
	struct sockaddr_in	in;
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
  host	= alloca(len);
  memcpy(host, adr, len);

  memset(sin, 0, sizeof *sin);
  for (s=host; *s; s++)
    if (*s==':')
      {
	*s	= 0;

	sin->in.sin_family	= AF_INET;
	sin->in.sin_addr.s_addr	= htonl(INADDR_ANY);
	sin->in.sin_port	= htons(atoi(s+1));

	if (s!=host && !inet_aton(host, &sin->in.sin_addr))
	  {
#ifdef	TINO_SOCK_NO_RESOLVE
	    tino_sock_error("%s", host));
#else
	    struct hostent	*he;

	    if ((he=gethostbyname(host))==0)
	      tino_sock_error("%s", host));
	    if (he->h_addrtype!=AF_INET || he->h_length!=sizeof sin->in.sin_addr)
	      tino_sock_error("unsupported host address"));
	    memcpy(&sin->in.sin_addr, he->h_addr, sizeof sin->in.sin_addr);
#endif
	  }
	return sizeof *sin;
      }

  sin->un.sun_family	= AF_UNIX;

  max = strlen(host);
  if (max > sizeof(sin->un.sun_path)-1)
    max = sizeof(sin->un.sun_path)-1;
  strncpy(sin->un.sun_path, host, max);

  return max + sizeof sin->un.sun_family;
}

static int
tino_sock_udp(const char *name, int do_listen)
{
  struct tino_sock_addr_gen	sa;
  int				sock;
  int				on;

  sock	= socket(AF_INET, SOCK_DGRAM, 0);

  on	= 102400;
  if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &on, sizeof on))
    tino_sock_error("setsockopt sndbuf"));

  on	= 102400;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &on, sizeof on))
    tino_sock_error("setsockopt rcvbuf"));

  tino_sock_getaddr(&sa, TINO_SOCK_UDP, name);

  if (bind(sock, (struct sockaddr *)&sa.addr.in, sizeof sa.addr.in))
    tino_sock_error("bind"));

  return sock;
}
#endif

static int
tino_sock_unix(const char *name, int do_listen)
{
  struct sockaddr_un	sun;
  int			sock;
  int			max;

  sun.sun_family    = AF_UNIX;

  max = strlen(name);
  if (max > sizeof(sun.sun_path)-1)
    max = sizeof(sun.sun_path)-1;
  strncpy(sun.sun_path, name, max);
  sun.sun_path[max]	= 0;

  max += sizeof sun.sun_family;

  sock	= socket(sun.sun_family, SOCK_STREAM, 0);
  if (sock<0)
    tino_sock_error("socket");

  if (do_listen>0)
    {
      umask(0);
      if (bind(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
	tino_sock_error("bind");

      if (listen(sock, do_listen))
	tino_sock_error("listen");
    }
  else if (connect(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
    tino_sock_error("connect");
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
tino_sock_get_adrname(union tino_sockaddr *sa, socklen_t sal)
{
  char	buf[256];

  switch (sa->sa.sa_family)
    {
    case AF_UNIX:
      TINO_XXX;	/* well, can we find out something?	*/
      return tino_str_printf("(unix:%s)", sa->un.sun_path);

#ifdef IPPROTO_IPV6
    case AF_INET6:
      if (!inet_ntop(sa->sa.sa_family, &sa->in6.sin6_addr, buf, sizeof buf))
	return 0;
      return tino_str_printf("[%s]:%ld", buf, ntohs(sa->in6.sin6_port));
#endif
    case AF_INET:
      if (!inet_ntop(sa->sa.sa_family, &sa->in.sin_addr, buf, sizeof buf))
	return 0;
      return tino_str_printf("%s:%ld", buf, ntohs(sa->in.sin_port));
    }
  return 0;
}

/* This is not yet thread-safe!
 * (Why did I write this warning? Looks pretty thread safe.)
 *
 * you must free the return value
 */
static char *
tino_sock_get_peername(int fd)
{
  union tino_sockaddr	sa;
  socklen_t		sal;

  sal	= sizeof sa;
  if (getpeername(fd, &sa.sa, &sal))
    return 0;
  return tino_sock_get_adrname(&sa, sal);
}

/* You must free the return value
 */
static char *
tino_sock_get_sockname(int fd)
{
  union tino_sockaddr	sa;
  socklen_t		sal;

  sal	= sizeof sa;
  if (getsockname(fd, &sa.sa, &sal))
    return 0;
  return tino_sock_get_adrname(&sa, sal);
}

static int
tino_sock_recv(int fd, void *buf, size_t len, union tino_sockaddr *adr)
{
  socklen_t	sl;
  int		got;

  sl	= sizeof *adr;
  got	= recvfrom(fd, buf, len, 0, (adr ? &adr->sa : NULL), &sl);
  return got;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/* This will be changed unconditionally from time to time.
 * Never even try to access it directly!
 */
#define	tino_sock_imp	tino_sock_imp_00234lkf9w89r2	/*AuToRoT*/

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

static inline int
tino_sock_state(TINO_SOCK sock)
{
  cDP(("tino_sock_state(%p) %d", sock, sock->state)); 
  return sock->state;
}

#if 0
static inline int
tino_sock_flags(TINO_SOCK sock)
{
  return sock->flags;
}
#endif

static inline int
tino_sock_fd(TINO_SOCK sock)
{
  cDP(("tino_sock_fd(%p) %d", sock, sock->fd)); 
  return sock->fd;
}

static inline void *
tino_sock_user(TINO_SOCK sock)
{
  cDP(("tino_sock_user(%p) %p", sock, sock->user)); 
  return sock->user;
}

static void
tino_sock_free_imp(TINO_SOCK sock)
{
  cDP(("tino_sock_free_imp(%p)", sock)); 

  sock->process		= 0;
  sock->user		= 0;
  sock->fd		= -1;
  sock->state		= TINO_SOCK_STATE_FREE;	/*  nice to have	*/

  sock->next		= tino_sock_imp.free;
  tino_sock_imp.free	= sock;
}

/* Warning, this has a sideeffect:
 */
static void
tino_sock_free(TINO_SOCK sock)
{
  cDP(("tino_sock_free(%p)", sock)); 

  if (!sock->process || sock->process(sock, TINO_SOCK_CLOSE)==TINO_SOCK_FREE)
    {
      if (sock->user)
	free(sock->user);
    }
  if (sock->fd)
    close(sock->fd);

  *sock->last	= sock->next;
  if (sock->next)
    sock->next->last	= sock->last;

  tino_sock_imp.use--;
  
  tino_sock_free_imp(sock);
}

static TINO_SOCK
tino_sock_new(int (*process)(TINO_SOCK, enum tino_sock_proctype),
	      void *user)
{
  TINO_SOCK	sock;

  cDP(("tino_sock_new(%p,%p)", process, user)); 

  if (!tino_sock_imp.free)
    {
      int	i;
      TINO_SOCK	tmp;

      tino_sock_imp.n	+= 16;
      tmp		=  tino_alloc(tino_sock_imp.n*sizeof *tmp);
      for (i=tino_sock_imp.n; --i>=0; )
	tino_sock_free_imp(tmp+i);
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

/* You *must* call this:
 * - after the socket has been created
 * - if the state of a socket may have changed
 * If you don't do it, your program will not work.
 * Hint: Use the forcepoll==1 on tino_sock_select to get all sockets polled.
 */
static TINO_SOCK
tino_sock_poll(TINO_SOCK sock)
{
  int	state;

  cDP(("tino_sock_poll(%p) state=%d", sock, sock->state)); 
  state	= sock->process(sock, sock->state<0 ? TINO_SOCK_EOF : TINO_SOCK_POLL);
  sock->state	= state;
  if (state<0)
    tino_sock_free(sock);
  cDP(("tino_sock_poll() state=%d", state)); 
  return sock;
}

static TINO_SOCK
tino_sock_new_fd(int fd,
		 int (*process)(TINO_SOCK, enum tino_sock_proctype),
		 void *user)
{
  TINO_SOCK	sock;

  cDP(("tino_sock_new_fd(%d,%p,%p)", fd, process, user)); 
  if (fd<0)
    tino_sock_error("sock new");
  sock		= tino_sock_new(process, user);
  sock->fd	= fd;
  cDP(("tino_sock_new_fd() %p", sock)); 
  return sock;
}

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
  cDP(("tino_sock_new_connect('%s',%p,%p)", target, process, user)); 
  return tino_sock_new_fd(tino_sock_tcp_connect(target, NULL), process, user);
}

static TINO_SOCK
tino_sock_new_listen(const char *bind,
		     int (*process)(TINO_SOCK, enum tino_sock_proctype),
		     void *user)
{
  cDP(("tino_sock_new_listen('%s',%p,%p)", bind, process, user)); 
  return tino_sock_new_fd(tino_sock_tcp_listen(bind), process, user);
}

/** Create a socketpair with standard parameters.
 */
static void
tino_sock_new_pair(int socks[2])
{
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks))
    tino_sock_error("socketpair");
}

static void
tino_sock_forcepoll(void)
{
  cDP(("tino_sock_forcepoll()")); 
  tino_sock_imp.forcepoll	= 1;
}

/* I know this needs a lot of optimization.
 *
 * By chance rewrite this for libevent.
 *
 * XXX add timeouts XXX
 *
 * Return value:
 * <0	error
 * 0	global EOF (no more sockets in use)
 * >0	something processed
 */
static int
tino_sock_select_timeout(int forcepoll, long timeout_ms, void (*alarm_fn)(void))
{
  TINO_SOCK		tmp;
  fd_set		r, w, e;
  int			loop, n;
  struct timeval	timeout;

  cDP(("tino_sock_select(%d)", forcepoll)); 
  if (tino_sock_imp.forcepoll)
    forcepoll	= 1;
  tino_sock_imp.forcepoll	= 0;

  for (loop=0;; loop++)
    {
      int	max;

      FD_ZERO(&r);
      FD_ZERO(&w);
      FD_ZERO(&e);
      max	= -1;
      for (tmp=tino_sock_imp.list; tmp; tmp=tmp->next)
	{
	  if (tmp->fd<0)
	    continue;
	  if (forcepoll)
	    tino_sock_poll(tmp);
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
      if (alarm_fn)
	alarm_fn();
      cDP(("tino_sock_select() select(%d,...)", max+1));
      if (max<0)
	return 0;
      TINO_XXX;	/* Add timeouts	*/
      if (timeout_ms>0)
	{
	  timeout.tv_sec	= timeout_ms/1000;
	  timeout.tv_usec	= (timeout_ms%1000)*1000;
	}
      if ((n=select(max+1, &r, &w, &e, (timeout_ms ? &timeout : NULL)))>0)
	break;
      cDP(("tino_sock_select() %d", n));
      if (!n)
	{
	  /* Timeout
	   */
	  TINO_XXX;
	  return n;
	}
      if ((errno!=EINTR && errno!=EAGAIN) || loop>1000)
	return n;
    }
  cDP(("tino_sock_select() %d", n));
  for (tmp=tino_sock_imp.list; tmp; tmp=tmp->next)
    if (tmp->fd>=0)
      {
	int	flag, nothing;

	cDP(("tino_sock_select() check %d", tmp->fd));
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
	    flag	= tmp->process(tmp, TINO_SOCK_READ);
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
	      tino_sock_free(tmp);
	    continue;
	  }
	if (!flag)
	  tmp->state	= TINO_SOCK_EOF;
	tino_sock_poll(tmp);
      }
  return n;
}

static int
tino_sock_select(int forcepoll)
{
  return tino_sock_select_timeout(forcepoll, 0l, NULL);
}

/* Do the standard looping.
 *
 * There is a checkfunc which is checked on each loop.
 *
 * returns -1:	terminate loop
 * return 
 */
static int
tino_sock_select_loop(int (*checkfunc)(void *), void *user)
{
  int	tmp;

  cDP(("tino_sock_select_loop(%p,%p)", checkfunc, user));
  do
    {
      tmp	= 0;
      if (checkfunc)
	{
	  cDP(("tino_sock_select_loop() checkfunc[%p](%p)", checkfunc, user));
	  if ((tmp=checkfunc(user))<0)
	    return tmp;
	}
    } while ((tmp=tino_sock_select(tmp))>0);

  if (tmp<0)
    tino_sock_error("tino_sock_select_loop select() error");
  cDP(("tino_sock_select_loop() ok"));
  return 0;
}

static int
tino_sock_use(void)
{
  cDP(("tino_sock_use() %d", tino_sock_imp.use));
  return tino_sock_imp.use;
}

/** Wrap stdin/stdout (blocking, nonselectable) sockets
 */
static int
tino_sock_wrap(int fd)
{
  pid_t	p;
  int	socks[2];

  tino_sock_new_pair(socks);
  if ((p=fork())==0)
    {
      char	buf[BUFSIZ];
      int	got, fdo=socks[2];

      alarm(0);
      for (i=OPEN_MAX; --i>=0; )
	if (i!=fd && i!=fdo)
	  tino_file_close(i);
      while ((got=tino_file_read(fd, buf, sizeof buf))>0)
	if (tino_file_write_all(fdo, buf, got)!=got)
	  exit(2);
      exit(got ? 1 : 0);
    }
  tino_file_close(socks[1]);
  tino_file_dup2(fd, tino_file_null());
  return socks[2];
}


#undef tino_sock_imp
#define tino_sock_imp	$tino_sock_imp_cannot_be_accessed$

#undef	cDP
#endif
