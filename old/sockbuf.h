/* $Header$
 *
 * Sockets with buffers.
 * This builds on top of tino_sock and tino_buf.
 * Shall use tino_ob later.
 *
 * $Log$
 * Revision 1.2  2004-07-17 22:23:09  tino
 * started to implement exceptions
 *
 * Revision 1.1  2004/06/13 03:49:29  tino
 * newly added
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
    int			(*close)(TINO_SOCKBUF);
    int			(*eof)(TINO_SOCKBUF);
    int			(*poll)(TINO_SOCKBUF);
    int			(*read)(TINO_SOCKBUF);
    int			(*write)(TINO_SOCKBUF);
    int			(*exception)(TINO_SOCKBUF);
    int			(*accept)(TINO_SOCKBUF);
    struct tino_buf	*in, *out;
  };

/* Accept new connections from socket
 */
static int
tino_sockbuf_process(TINO_SOCK sock, enum tino_sock_proctype type)
{
  TINO_SOCKBUF	*p=tino_sock_user(sock);
  int		fd;

  FATAL(sock!=p->sock);
  switch (type)
    {
    default:
      tino_fatal("tino_sockbuf_process type=%d unknown", type);

    case TINO_SOCK_PROC_CLOSE:
      return p->close(p);
      
    case TINO_SOCK_PROC_EOF:
      return p->eof(p);

    case TINO_SOCK_PROC_POLL:
      return p->poll(p);

    case TINO_SOCK_PROC_READ:
      return p->read(p);

    case TINO_SOCK_PROC_WRITE:
      return p->write(p);

    case TINO_SOCK_PROC_EXCEPTION:
      return p->exception(p);

    case TINO_SOCK_PROC_ACCEPT:
      return p->accept(p);
    }
}

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
