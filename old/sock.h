/* $Header$
 *
 * $Log$
 * Revision 1.3  2004-05-19 05:00:04  tino
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
 * If the function returns <0 the socket will enter TINO_SOCK_STATE_ERR
 * If the function returns 0 on READ we have EOF on READ.
 * If the function returns 0 on WRITE we have EOF on WRITE.
 *
 * If the function is not defined, and user!=0 then the user-pointer
 * is destroyed on tino_sock_free, This means free() or tino_ob_destroy().
 */
enum tino_sock_proctype
  {
    TINO_SOCK_CLOSE	= -1,	/* perform close (cleanup user pointer)	*/
    TINO_SOCK_POLL	= 0,	/* returns bitmask of following:	*/
    TINO_SOCK_READ	= 1,
    TINO_SOCK_WRITE	= 2,
    TINO_SOCK_EXCEPTION	= 4,
    TINO_SOCK_ACCEPT	= 8,
  };

enum tino_sock_state
  {
    TINO_SOCK_ERR	= -2,
    TINO_SOCK_EOF	= -1,
    TINO_SOCK_IDLE	= 0,
    TINO_SOCK_READ	= 1,	/* read() on socket allowed	*/
    TINO_SOCK_WRITE	= 2,	/* write() on socket allowed	*/
    TINO_SOCK_READWRITE	= 3,	/* both allowed			*/
    TINO_SOCK_ACCEPT	= 8,	/* accept() allowed		*/
  };

enum tino_sock_flags
  {
    TINO_SOCK_READ	= 1,	/* read() supported	*/
    TINO_SOCK_WRITE	= 2,	/* write() supported	*/
    TINO_SOCK_EXCEPTION	= 4,	/* exception (OOB data) supported	*/
    TINO_SOCK_ACCEPT	= 8,	/* accept() supported	*/
  };

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
    TINO_SOCK	*socks, free;
  } tino_sock_imp;

struct tino_sock
  {
    TINO_SOCK		next, *last;
    int			state, flags;
    int			fd;
    int			(*process)(struct tino_sock *, int);
    void		*user;
  };

static inline int
tino_sock_state(TINO_SOCK sock)
{
  return sock->state;
}

static inline int
tino_sock_flags(TINO_SOCK sock)
{
  return sock->flags;
}

static inline int
tino_sock_fd(TINO_SOCK sock)
{
  return sock->fd;
}

static TINO_SOCK
tino_sock_new(int (*process)(TINO_SOCK, enum tino_sock_proctype), void *user)
{
  if (!tino_sock_imp->free)
    {
    }
}

static void
tino_sock_free(TINO_SOCK sock)
{
  
}

static TINO_SOCK
tino_sock_new_fd(int fd, int (*process)(TINO_SOCK, enum tino_sock_proctype), void *user)
{
  TINO_SOCK	sock;

  sock	= tino_sock_new(user);
  
  ptr		= tino_alloc(sizeof *ptr);
  ptr->next	= 0;
  ptr->last	= 0;
  ptr->fd	= fd;
  ptr->process	= process;
  ptr->user	= user;
  return ptr;
}





#endif
