/* $Header$
 *
 * Sockets with buffers
 * This builds on top of tino_sock and tino_buf
 * Will be moved into sockbuf.h later on, I think.
 *
 * $Log$
 * Revision 1.1  2004-06-13 03:49:29  tino
 * newly added
 *
 */

#ifndef tino_INC_sockbuf_h
#define tino_INC_sockbuf_h

#include "buf.h"
#include "sock.h"

typedef struct tino_sockbuf *TINO_SOCKBUF;

struct tino_sockbuf
  {
    TINO_SOCK		*sock;
    void		*user;
    struct tino_buf	*in, *out;
  };

static TINO_SOCKBUF
tino_sockbuf_new(int fd, void *user)
{
  TINO_SOCK	*sock;

  
  if (fd<0)
    sock	= tino_sock_new(tino_sockbuf_process, sb);
  else
    sock	= tino_sock_new_fd(fd, tino_sockbuf_process, sb);
  sb->sock	= sock;
}

#endif
