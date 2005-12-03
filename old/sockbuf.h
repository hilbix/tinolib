/* $Header$
 *
 * Sockets with buffers.
 * This builds on top of tino_sock and tino_buf.
 * Shall use tino_ob later.
 *
 * $Log$
 * Revision 1.4  2005-12-03 12:53:14  tino
 * Still is incomplete and nearly untested, but usable now.
 *
 * Revision 1.3  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.2  2004/07/17 22:23:09  tino
 * started to implement exceptions
 *
 * Revision 1.1  2004/06/13 03:49:29  tino
 * newly added
 */

#ifndef tino_INC_sockbuf_h
#define tino_INC_sockbuf_h

#include "buf.h"
#include "sock.h"

#define	cDP	TINO_DP_sock

typedef struct tino_sockbuf *TINO_SOCKBUF;

struct tino_sockbuf_fn
  {
    int		(*accept	)(TINO_SOCKBUF);
    int		(*poll		)(TINO_SOCKBUF);
    int		(*read		)(TINO_SOCKBUF);
    void	(*write_hook	)(TINO_SOCKBUF, int);
    int		(*write		)(TINO_SOCKBUF);
    int		(*eof		)(TINO_SOCKBUF);
    int		(*exception	)(TINO_SOCKBUF);
    void	(*close		)(TINO_SOCKBUF);
  };

#define	TINO_SOCKBUF_SET(BUF,WHAT,FN)	do { (BUF)->fn.WHAT=(FN); } while (0)

#define TINO_SOCKBUF_ACCEPT	accept
#define TINO_SOCKBUF_POLL	poll
#define TINO_SOCKBUF_READ	read
#define TINO_SOCKBUF_WRITE_HOOK	write_hook
#define TINO_SOCKBUF_WRITE	write
#define TINO_SOCKBUF_EOF	eof
#define TINO_SOCKBUF_EXCEPTION	exception
#define TINO_SOCKBUF_CLOSE	close

struct tino_sockbuf
  {
    TINO_SOCK			sock;
    void			*user;
    struct tino_sockbuf_fn	fn;
    TINO_BUF			in, out;
    TINO_SOCKBUF		next, prev;
  };

inline TINO_BUF *
tino_sockbuf_in(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("tino_sockbuf_in(%p) %p", buf, (buf->next ? &buf->next->out : &buf->in)));
  return (buf->next ? &buf->next->out : &buf->in);
}

inline TINO_BUF *
tino_sockbuf_out(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("tino_sockbuf_out(%p) %p", buf, &buf->out));
  return &buf->out;
}

/* Accept new connections from socket
 */
static int
tino_sockbuf_process(TINO_SOCK sock, enum tino_sock_proctype type)
{
  TINO_SOCKBUF	p=tino_sock_user(sock);

  cDP(("tino_sockbuf_process(%p, %d)", p, type));
  tino_FATAL(sock!=p->sock);
  switch (type)
    {
      TINO_SOCK	tmp;
      int	ret;

    default:
      tino_fatal("tino_sockbuf_process type=%d unknown", type);

    case TINO_SOCK_PROC_CLOSE:
      cDP(("tino_sockbuf_process() CLOSE"));
      if (p->fn.close)
	p->fn.close(p);
      tino_buf_free(&p->in);
      tino_buf_free(&p->out);
      if (p->next)
	p->next->prev	= 0;
      p->next		= 0;
      if (p->prev)
	p->prev->next	= 0;
      p->prev		= 0;
      return TINO_SOCK_FREE;

    case TINO_SOCK_PROC_EOF:
      cDP(("tino_sockbuf_process() EOF"));
      if (p->fn.eof)
	return p->fn.eof(p);
      if (p->prev)
	{
	  tmp		= p->prev->sock;
	  p->prev->next	= 0;
	  p->prev	= 0;
	  tino_sock_poll(tmp);
	}
      return TINO_SOCK_FREE;

    case TINO_SOCK_PROC_POLL:
      cDP(("tino_sockbuf_process() POLL"));
      if (p->fn.poll)
	return p->fn.poll(p);
      if (p->prev && tino_sock_state(p->prev->sock)<0)
	return TINO_SOCK_EOF;
      return ((tino_buf_get_len(tino_sockbuf_out(p)) ? TINO_SOCK_WRITE : 0) |
	      (tino_buf_get_len(tino_sockbuf_in(p)) ? 0 : TINO_SOCK_READ));

    case TINO_SOCK_PROC_READ:
      cDP(("tino_sockbuf_process() READ"));
      if (p->fn.read)
	return p->fn.read(p);
      ret	= tino_buf_read(tino_sockbuf_in(p), tino_sock_fd(sock), -1);
      if (p->next)
	tino_sock_poll(p->next->sock);
      return ret;

    case TINO_SOCK_PROC_WRITE:
      cDP(("tino_sockbuf_process() WRITE"));
      if (p->fn.write)
	return p->fn.write(p);
      ret	= tino_buf_write_away(tino_sockbuf_out(p), tino_sock_fd(sock));
      if (p->fn.write_hook)
	p->fn.write_hook(p, ret-1);
      if (p->prev)
	tino_sock_poll(p->prev->sock);
      return ret;

    case TINO_SOCK_PROC_EXCEPTION:
      cDP(("tino_sockbuf_process() EXCEPTION"));
      if (p->fn.exception)
	return p->fn.exception(p);
      break;

    case TINO_SOCK_PROC_ACCEPT:
      cDP(("tino_sockbuf_process() ACCEPT"));
      if (p->fn.accept)
	return p->fn.accept(p);
      break;
    }
  tino_sock_error("tino_sockbuf_process: not handled %d", type);
  return -1;
}

static TINO_SOCKBUF
tino_sockbuf_new(int fd, void *user)
{
  TINO_SOCK	sock;
  TINO_SOCKBUF	sb;

  cDP(("tino_sockbuf_new(%d, %p)", fd, user));
  sb		= tino_alloc0(sizeof *sb);
  tino_buf_init(&sb->in);
  tino_buf_init(&sb->out);
  sb->user	= user;
  sb->next	= 0;
  if (fd<0)
    sock	= tino_sock_new(tino_sockbuf_process, sb);
  else
    sock	= tino_sock_new_fd(fd, tino_sockbuf_process, sb);
  sb->sock	= sock;
  cDP(("tino_sockbuf_new() %p", sb));
  return sb;
}

static TINO_SOCKBUF
tino_sockbuf_new_connect(const char *target, void *user)
{
  cDP(("tino_sockbuf_new_connect('%s', %p)", target, user));
  return tino_sockbuf_new(tino_sock_tcp_connect(target, NULL), user);
}

static TINO_SOCKBUF
tino_sockbuf_new_listen(const char *bind, void *user)
{
  cDP(("tino_sockbuf_new_listen('%s', %p)", bind, user));
  return tino_sockbuf_new(tino_sock_tcp_listen(bind), user);
}

static TINO_SOCKBUF
tino_sockbuf_set(TINO_SOCKBUF buf, struct tino_sockbuf_fn *fn)
{
  cDP(("tino_sockbuf_set(%p, %p)", buf, fn));
  tino_FATAL(!buf);
  buf->fn	= *fn;
  return buf;
}

static void
tino_sockbuf_next(TINO_SOCKBUF buf, TINO_SOCKBUF next)
{
  cDP(("tino_sockbuf_next(%p,%p)", buf, next));
  tino_FATAL(!buf);
  if (buf->next)
    buf->next->prev	= 0;
  buf->next	= next;
  if (next)
    next->prev	= buf;
}

static void *
tino_sockbuf_user(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("tino_sockbuf_user(%p) %p", buf, buf->user));
  return buf->user;
}

#undef	cDP
#endif
