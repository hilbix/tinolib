/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-05-01 01:41:06  tino
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

static int
tino_socket_unix(const char *name, int do_connect)
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

  if (!connect)
    {
      umask(0);
      if (bind(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
	ex("bind");

      if (listen(sock, 10))
	ex("listen");
    }
  else if (connect(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
    ex("connect");
  return sock;
}

static int
tino_socket_unix_connect(const char *name)
{
  return tino_socket_unix(name, 1);
}

static int
tino_socket_unix_listen(const char *name)
{
  return tino_socket_unix(name, 0);
}
#endif
