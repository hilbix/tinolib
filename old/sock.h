/* $Header$
 *
 * This is far from ready yet.
 *
 * $Log$
 * Revision 1.8  2004-06-13 03:41:52  tino
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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ex.h"

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
#if 0
    TINO_SOCK_EXCEPTION	= 4,
#endif
    TINO_SOCK_ACCEPT	= 8,
  };

enum tino_sock_proctype
  {
    TINO_SOCK_PROC_CLOSE	= TINO_SOCK_CLOSE,	/* close (free user) */
    TINO_SOCK_PROC_EOF		= TINO_SOCK_EOF,	/* EOF encountered, */
    TINO_SOCK_PROC_POLL		= TINO_SOCK_POLL,	/* return bitmask: */
    TINO_SOCK_PROC_READ		= TINO_SOCK_READ,
    TINO_SOCK_PROC_WRITE	= TINO_SOCK_WRITE,
#if 0
    TINO_SOCK_PROC_EXCEPTION	= TINO_SOCK_EXCEPTION,
#endif
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
#if 0
    TINO_SOCK_UNIX_DGRAM	=2,
#endif
    TINO_SOCK_TCP		=3,
    TINO_SOCK_UDP		=4,
  };

/* Open a socket of given type.
 * If type is AUTO this function tries to autodetects the type of socket.
 *
 * Autodetection is done as follows:
 * "unix:"	Unix-type stream socket
 * "dgram:"	Unix-type dgram socket
 * "tcp:"	TCP-type socket (stream)
 * "udp:"	UDP-type socket (dgram)
 * ""
 */
static int
tino_sock_getaddr(union tino_sockaddr_gen *sin, int typ, const char *adr, ...)
{
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
	    ex(host);
#else
	    struct hostent	*he;

	    if ((he=gethostbyname(host))==0)
	      ex(host);
	    if (he->h_addrtype!=AF_INET || he->h_length!=sizeof sin->in.sin_addr)
	      ex("unsupported host address");
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
    ex("socket");

  if (do_listen>0)
    {
      umask(0);
      if (bind(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
	ex("bind");

      if (listen(sock, do_listen))
	ex("listen");
    }
  else if (connect(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
    ex("connect");
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
    int		n;
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

  sock->state		= 0;
#if 0
  sock->flags		= 0;
#endif
  sock->next		= tino_sock_imp.list;
  tino_sock_imp.list	= sock;
  sock->last		= 0;
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
    ex("sock new");
  sock		= tino_sock_new(process, user);
  sock->fd	= fd;
  return sock;
}

/* I know this needs a lot of optimization.
 *
 * By chance rewrite this for libevent.
 */
static int
tino_sock_select(int forcepoll)
{
  TINO_SOCK		tmp;
  fd_set		r, w;
  int			i, loop, n;

  xDP(("tino_sock_select(%d)", forcepoll));
  for (loop=0;; loop++)
    {
      int	max;

      FD_ZERO(&r);
      FD_ZERO(&w);
      max	= 0;
      for (i=tino_sock_imp.n, tmp=tino_sock_imp.socks; --i>=0; tmp++)
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
	      if (tmp->fd>max)
		max	= tmp->fd;
	    }
	}
      if ((n=select(max+1, &r, &w, NULL, NULL))>0)
	break;
      xDP(("tino_sock_select() %d", n));
      if (!n)
	{
	  /* Timeout
	   */
	  000;
	  FATAL(!n);
	}
      if ((errno!=EINTR && errno!=EAGAIN) || loop>1000)
	return n;
    }
  xDP(("tino_sock_select() %d", n));
  for (i=tino_sock_imp.n, tmp=tino_sock_imp.socks; --i>=0; tmp++)
    if (tmp->fd>=0)
      {
	int	flag;

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
	if (FD_ISSET(tmp->fd, &r))
	  flag	= tmp->process(tmp, TINO_SOCK_READ);
	else if (FD_ISSET(tmp->fd, &w))
	  flag	= tmp->process(tmp, TINO_SOCK_WRITE);
	else
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
