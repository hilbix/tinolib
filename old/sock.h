/* $Header$
 *
 * This is far from ready yet.
 *
 * This will sometimes be merged in the successor: io.h
 *
 * $Log$
 * Revision 1.16  2005-06-28 20:10:28  tino
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

#include <unistd.h>
#include <netdb.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>


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
    TINO_SOCK_FREE	= -3,
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
    TINO_SOCK_PROC_CLOSE	= TINO_SOCK_CLOSE,	/* close (free user) */
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
 */

TINO_THREAD_SEMAPHORE(tino_sock_sem);

typedef void (*tino_sock_error_fn_t)(const char *err, va_list list);
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
  va_list	list;

  va_start(list, err);
  if (tino_sock_error_fn)
    tino_sock_error_fn(err, list);
  else
    TINO_VEXIT((err, list));
  tino_fatal("tino_sock_error returns");
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
union sockaddr_gen
   {
     struct sockaddr		sa;
     struct sockaddr_un		un;
     struct sockaddr_in		in;
#ifdef IPPROTO_IPV6
     struct sockaddr_in6	in6;
#endif
  };

static int
tino_sock_getaddr(union sockaddr_gen *sin, const char *adr)
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
  union sockaddr_gen	sa;
  int			on, len;
  int			sock;

  len	= tino_sock_getaddr(&sa, to);

  sock	= socket(sa.sa.sa_family, SOCK_STREAM, 0);
  if (sock<0)
    tino_sock_error("socket");

  on = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on))
    tino_sock_error("setsockopt reuse");

  if (local)
    {
      union sockaddr_gen	l_sa;
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
  union sockaddr_gen	sin;
  int			on, len;
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
  on = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on))
    tino_sock_error("setsockopt reuse");

  if (bind(sock, &sin.sa, len))
    tino_sock_error("bind");

  if (listen(sock, 100))
    tino_sock_error("listen");

  return sock;
}
/* END COPY
 */
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
tino_sock_getaddr(union tino_sockaddr_gen *sin, int type, const char *adr)
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


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static struct tino_sock_imp
  {
    int		n, use;
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
  return sock->fd;
}

static inline void *
tino_sock_user(TINO_SOCK sock)
{
  return sock->user;
}

static void
tino_sock_free_imp(TINO_SOCK sock)
{
  sock->process		= 0;
  sock->user		= 0;
  sock->fd		= -1;

  sock->next		= tino_sock_imp.free;
  tino_sock_imp.free	= sock;
}

static void
tino_sock_free(TINO_SOCK sock)
{
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

  sock->state		= 0;
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

  state	= sock->process(sock, sock->state<0 ? TINO_SOCK_EOF : TINO_SOCK_POLL);
  sock->state	= state;
  if (state<0)
    tino_sock_free(sock);
  return sock;
}

static TINO_SOCK
tino_sock_new_fd(int fd,
		 int (*process)(TINO_SOCK, enum tino_sock_proctype),
		 void *user)
{
  TINO_SOCK	sock;

  if (fd<0)
    TINO_EXIT(("sock new"));
  sock		= tino_sock_new(process, user);
  sock->fd	= fd;
  return sock;
}

/* I know this needs a lot of optimization.
 *
 * By chance rewrite this for libevent.
 *
 * XXX add timeouts XXX
 */
static int
tino_sock_select(int forcepoll)
{
  TINO_SOCK		tmp;
  fd_set		r, w, e;
  int			loop, n;

  xDP(("tino_sock_select(%d)", forcepoll));
  for (loop=0;; loop++)
    {
      int	max;

      FD_ZERO(&r);
      FD_ZERO(&w);
      FD_ZERO(&e);
      max	= 0;
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
      000;	/* XXX Add timeouts	*/
      if ((n=select(max+1, &r, &w, &e, NULL))>0)
	break;
      xDP(("tino_sock_select() %d", n));
      if (!n)
	{
	  /* Timeout
	   */
	  000;
	  tino_FATAL(!n);
	}
      if ((errno!=EINTR && errno!=EAGAIN) || loop>1000)
	return n;
    }
  xDP(("tino_sock_select() %d", n));
  for (tmp=tino_sock_imp.list; tmp; tmp=tmp->next)
    if (tmp->fd>=0)
      {
	int	flag, nothing;

	xDP(("tino_sock_select() check %d", tmp->fd));
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

#endif
