/* Socket functions
 * Examples see my tools like ptybuffer/metaproxy/udptx
 *
 * This is far from ready yet.
 * This will sometimes be merged in the successor: io.h
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
#include <stddef.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
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

/** Function to ignore (return) socket errors
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

static int
tino_sock_nodelayE(int sock, int onoff)
{
  cDP(("(%d,%d)", sock, onoff));
  return TINO_F_setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &onoff, sizeof onoff);
}

static void
tino_sock_nodelayA(int sock, int onoff)
{
  cDP(("(%d,%d)", sock, onoff));
  if (tino_sock_nodelayE(sock, onoff))
    tino_sock_error("setsockopt(TCP_NODELAY)");
}

#ifndef TCP_CORK
#ifdef TCP_NOPUSH
#define TCP_CORK TCP_NOPUSH
#endif
#endif
#ifdef TCP_CORK
static int
tino_sock_corkE(int sock, int onoff)
{
  cDP(("(%d,%d)", sock, onoff));
  return TINO_F_setsockopt(sock, IPPROTO_TCP, TCP_CORK, &onoff, sizeof onoff);
}

static void
tino_sock_corkA(int sock, int onoff)
{
  cDP(("(%d,%d)", sock, onoff));
  if (tino_sock_corkE(sock, onoff))
    tino_sock_error("setsockopt(TCP_CORK/TCP_NOPUSH)");
}
#endif

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
 * unix domain socket.  If the unix domain socket starts with @,
 * it is considered an Abstract Linux Socket.
 *
 * Note on IPv4 vs. IPv6:
 * - You can give the address  as IP:port, defaults to IPv6 if a : is present in IP, else IPv4
 * - You can give the address  as [IP]:port, defaults to IPv6 with no fallback to IPv4
 * - You can give the address  as host:port, defaults to IPv4 if IPv4 address exists, else IPv6
 * - You can give the address  as [host]:port, defaults to IPv6 with no fallback to IPv4
 * - You can give 0.0.0.0:port as :port
 * - You can give :::port      as []:port
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
  /* @abstract
   * /path/to/unix
   * ./relative/path/to/unix
   * host:port
   */
  if (s && *host!='@' && *host!='/' && *host!='.')
    {
      int	family;	/* 4 or 6	*/

      *s	= 0;

      /* How to make this future-proof for later protocols like IPv6 and/or how to support other protocols here?
       * There should be a generic proven and usable way how to specify some protocol address!
       * (like AX.25)
       * and some portable way to compile it without need for #ifdef like seen below!
       */
      family	= 6;
      if (*host == '[')
        {
          len	= strlen(host);
          if (host[len-1] != ']')
            {
              /* sin->len	= 0;	still is 0 here	*/
              return tino_sock_error("missing ']' in %s", host);
            }
          host[len-1]	= 0;
          host++;
        }
      else if (!strchr(host, ':'))
        family	= 4;

#ifdef	TINO_HAS_IPv6
      /* WTF why has IPv6 an incompatible memory layout compared to IPv4?
       * offsetof(.sin6_addr) !== offsetof(.sin_addr)
       *
       * BTW, what is .sin6_flowinfo?  It's entirely undocumented!
       * Telling it is not implemented or naming the field "IPv6 traffic class and flow information" (RFC)
       * or "IPv6 flow identifier" (Kernel) makes things even worse.
       *
       * RFC 2553 says "The contents and interpretation of this member is specified in RFC 2460" but it isn't.
       * RFC 2460 explains Flow Labels and Traffic Class, but no word on how to make up the flowinfo out of these!
       * Flowinfo is 32 bit.  Traffic class is 8 bit.  Flow label is 20 bit.  This simply does not fit.
       * As everything in SA is network byte order, it is probably just a copy of the first 32 bit of the IPv6 header.
       * But that is illogical, because between this and the destination address there are 128+32 other bits.
       *
       * It would be logical if the IPv6 header would be:
       * 4 Bit Version
       * 8 Bit Traffic Class
       * 20 Bit Flow Label
       * 128 Bit Destination address
       *
       * Then it would make a bit of a sense to stuff flowinfo it before the address.
       * But this is not how the IPv6 header looks like.  Hence stuffing it in front just makes things go bad.
       * It does not make sense at all, as there also is .sin6_scope_id which goes AFTER the address.
       * I'd suggest that additional fields either go BEFORE the address or AFTER but never both!
       */
      if (family == 6)
        {
          sin->sa.in6.sin6_family	= AF_INET6;
          sin->sa.in6.sin6_addr		= in6addr_any;
          sin->sa.in6.sin6_port		= TINO_F_htons(atoi(s+1));
          sin->len			= sizeof sin->sa.in6;
        }
      else
#endif
        {
          sin->sa.in.sin_family		= AF_INET;
          sin->sa.in.sin_addr.s_addr	= INADDR_ANY;
          sin->sa.in.sin_port		= TINO_F_htons(atoi(s+1));
          sin->len			= sizeof sin->sa.in;
        }

      if (*host)
        {
          struct addrinfo	*ai, hints, *tmp;
          int			rc;

          /* sadly gethostbyname is not portable to IPv6.
           * I really do not understand why, as IPv4 can be mapped to IPv6,
           * but apparently the people did not support it that way.
           * SIGH
           * So we have to re-invent some complex wheel here instead
           * of using just some standard lib default.
           *
           * Note that we can only resolve a single address here.
           * So we use the first one we get hold on.
           */

          hints.ai_flags	= AI_ADDRCONFIG | AI_NUMERICSERV;	/* WTF!?!?!  How to get the system default for this?	*/
          hints.ai_socktype	= SOCK_STREAM;
          hints.ai_family	= AF_UNSPEC;
#ifdef	TINO_HAS_IPv6
          if (family==6)
            {
              hints.ai_family	= AF_INET6;
              hints.ai_flags	|= AI_V4MAPPED;				/* WTF!?!?!	*/
            }
#endif

          ai	= 0;
          rc	= getaddrinfo(host, s+1, &hints, &ai);
          if (rc)
            {
              const char *err;

              err = gai_strerror(rc);
              if (rc != EAI_SYSTEM)	/* WTF?!?	*/
                errno	= ENXIO;

              if (ai)
                freeaddrinfo(ai);
              sin->len	= 0;
              return tino_sock_error("cannot resolve %s: %s", host, err);
            }


          tmp	= 0;

#ifdef	TINO_HAS_IPv6
          family	= family==6 ? AF_INET6 : AF_INET;			 /* WTF!?!?! How to make this future-proof?	*/
          for (tmp=ai; tmp; tmp=tmp->ai_next)
            if (tmp->ai_family == family)
              break;
#endif

          if (!tmp)
            tmp	= ai;

          xDP(("(%p: %d, %d)", tmp, tmp->ai_family, family));

          len	= tmp->ai_addrlen;
          if (len>sizeof *sin)
            {
              errno	= ENXIO;
              freeaddrinfo(ai);
              sin->len = 0;
              return tino_sock_error("unsupported host address length: %ld, maximum %ld", (long)len, (long)sizeof *sin);
            }
          memcpy(&sin->sa, tmp->ai_addr, len);
          sin->len	= len;
          /* sin->sa.in.sin_port	= TINO_F_htons(atoi(s+1));	*/

          freeaddrinfo(ai);
        }
      return 0;
    }

  sin->sa.un.sun_family	= AF_UNIX;

  max = strlen(host);
  if (max > (int)sizeof(sin->sa.un.sun_path))
    {
      errno	= ENAMETOOLONG;
      sin->len	= 0;
      return tino_sock_error("path too long: %s", host);
    }
  memcpy(sin->sa.un.sun_path, host, max);
  sin->len	= max + offsetof(tino_sockaddr_t,sa.un.sun_path);
  if (host[0]=='@')
    sin->sa.un.sun_path[0]	= 0;	/* Abstract Linux Socket	*/
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
tino_sock_tcp_listen_hook(const char *s, int backlog, int (*hook)(int fd, void *user), void *user)
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

  if (hook)
    {
      sock	 = hook(sock, user);
      if (sock<0)
        return -1;
    }

  if (TINO_F_bind(sock, &sin.sa.sa, sin.len))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("bind");
    }

  if (TINO_F_listen(sock, backlog>0 ? backlog : 100))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("listen");
    }

  return sock;
}

static int
tino_sock_tcp_listen(const char *s)
{
  return tino_sock_tcp_listen_hook(s, 0, NULL, NULL);
}

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

/** NOT READY YET
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

/* perhaps set umask() before calling this	*/
static int
tino_sock_unixAi(const char *name, int do_listen)
{
  TINO_T_sockaddr_un	sun;
  int			sock;
  int			max;

  sun.sun_family    = AF_UNIX;

  max = strlen(name);
  if (max > (int)sizeof(sun.sun_path))
    return tino_sock_error("path too long: %s", name);

  memcpy(sun.sun_path, name, max);
  if (name[0]=='@')
    sun.sun_path[0]	= 0;	/* Abstract Linux Socket	*/

  max	+= offsetof(TINO_T_sockaddr_un,sun_path);

  sock	= TINO_F_socket(sun.sun_family, SOCK_STREAM, 0);
  if (sock<0)
    return tino_sock_error("socket");

  if (do_listen>0)
    {
#if 0
      umask(0);	/* actually, this seems to be a bug to me	*/
#endif
      if (TINO_F_bind(sock, (TINO_T_sockaddr *)&sun, max))
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
  else if (TINO_F_connect(sock, (TINO_T_sockaddr *)&sun, max))
    {
      tino_file_close_ignO(sock);
      return tino_sock_error("connect");
    }
  return sock;
}

static int
tino_sock_unix_connect(const char *name)
{
  return tino_sock_unixAi(name, 0);
}

/* perhaps set umask() before calling this	*/
static int
tino_sock_unix_listenAi(const char *name)
{
  return tino_sock_unixAi(name, 1);
}

static char *
tino_sock_get_adrnameN(tino_sockaddr_t *sa)
{
  char	buf[256];

  switch (sa->sa.sa.sa_family)
    {
    case AF_UNIX:
      if (sa->sa.un.sun_path[0]==0)	/* Abstract Linux Socket	*/
        return tino_str_printf("(unix:@%.*s)", (int)sa->len-offsetof(tino_sockaddr_t,sa.un.sun_path)-1, sa->sa.un.sun_path+1);
      TINO_XXX;	/* well, can we find out something?	*/
      return tino_str_printf("(unix:%.*s)", (int)sa->len-offsetof(tino_sockaddr_t,sa.un.sun_path), sa->sa.un.sun_path);

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

/** You must free the return value
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

/** You must free the return value
 */
static char *
tino_sock_get_socknameN(int fd)
{
  tino_sockaddr_t	sa;

  sa.len	= sizeof sa.sa;
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
