/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-04-18 14:18:05  tino
 * added, as I need some standard socket things
 *
 */

#ifndef tino_INC_socket_h
#define tino_INC_socket_h

#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ex.h"

static int
tino_socket_unix(const char *name)
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

  umask(0);
  if (bind(sock, (struct sockaddr *)&sun, max+sizeof sun.sun_family))
    ex("bind");

  if (listen(sock, 10))
    ex("listen");

  return sock;
}

#endif
