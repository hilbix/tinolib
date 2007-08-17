/* $Header$
 *
 * Sockets with buffers.
 * This builds on top of tino_sock and tino_buf.
 * Shall use tino_ob later.
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.10  2007-08-17 18:26:21  tino
 * See ChangeLog
 *
 * Revision 1.9  2007/08/06 15:46:00  tino
 * Changed to use current prototypes
 *
 * Revision 1.8  2007/04/22 21:29:50  tino
 * Debug comment corrected
 *
 * Revision 1.7  2006/07/22 17:30:13  tino
 * two functions added
 *
 * Revision 1.6  2006/01/07 18:05:28  tino
 * tino_buf_write_away changed and read_hook added
 *
 * Revision 1.5  2005/12/05 02:09:20  tino
 * ->name added etc.
 *
 * Revision 1.4  2005/12/03 12:53:14  tino
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

#include "sockgen.h"
#include "buf.h"

#define	cDP	TINO_DP_sock

typedef struct tino_sockbuf *TINO_SOCKBUF;

struct tino_sockbuf_fn
  {
    int		(*accept	)(TINO_SOCKBUF, int);
    int		(*poll		)(TINO_SOCKBUF);
    int		(*read		)(TINO_SOCKBUF);
    void	(*read_hook	)(TINO_SOCKBUF, int);	/* do not alter errno!	*/
    int		(*write		)(TINO_SOCKBUF);
    void	(*write_hook	)(TINO_SOCKBUF, int);	/* do not alter errno!	*/
    int		(*eof		)(TINO_SOCKBUF);
    int		(*exception	)(TINO_SOCKBUF);
    void	(*close		)(TINO_SOCKBUF);
  };

#define	TINO_SOCKBUF_SET(BUF,WHAT,FN)	do { (BUF)->fn.WHAT=(FN); } while (0)

#define TINO_SOCKBUF_ACCEPT	accept
#define TINO_SOCKBUF_POLL	poll
#define TINO_SOCKBUF_READ	read
#define TINO_SOCKBUF_READ_HOOK	read_hook
#define TINO_SOCKBUF_WRITE	write
#define TINO_SOCKBUF_WRITE_HOOK	write_hook
#define TINO_SOCKBUF_EOF	eof
#define TINO_SOCKBUF_EXCEPTION	exception
#define TINO_SOCKBUF_CLOSE	close

struct tino_sockbuf
  {
    TINO_SOCK			sock;
    void			*user;
    char			*name;
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
  TINO_SOCKBUF	p=tino_sock_userO(sock);

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
      free(p->name);
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
	  tino_sock_pollOn(tmp);
	}
      return TINO_SOCK_FREE;

    case TINO_SOCK_PROC_POLL:
      cDP(("tino_sockbuf_process() POLL"));
      if (p->fn.poll)
	return p->fn.poll(p);
      if (p->prev && tino_sock_stateO(p->prev->sock)<0)
	return TINO_SOCK_EOF;
      return ((tino_buf_get_len(tino_sockbuf_out(p)) ? TINO_SOCK_WRITE : 0) |
	      (tino_buf_get_len(tino_sockbuf_in(p)) ? 0 : TINO_SOCK_READ));

    case TINO_SOCK_PROC_READ:
      cDP(("tino_sockbuf_process() READ"));
      if (p->fn.read)
	return p->fn.read(p);
      ret	= tino_buf_read(tino_sockbuf_in(p), tino_sock_fdO(sock), -1);
      if (p->fn.read_hook)
	p->fn.read_hook(p, ret);
      if (p->next)
	tino_sock_pollOn(p->next->sock);
      return ret;

    case TINO_SOCK_PROC_WRITE:
      cDP(("tino_sockbuf_process() WRITE"));
      if (p->fn.write)
	return p->fn.write(p);
      ret	= tino_buf_write_away(tino_sockbuf_out(p), tino_sock_fdO(sock), -1);
      if (p->fn.write_hook)
	p->fn.write_hook(p, ret);
      if (p->prev)
	tino_sock_pollOn(p->prev->sock);
      return ret;

    case TINO_SOCK_PROC_EXCEPTION:
      cDP(("tino_sockbuf_process() EXCEPTION"));
      if (p->fn.exception)
	return p->fn.exception(p);
      break;

    case TINO_SOCK_PROC_ACCEPT:
      cDP(("tino_sockbuf_process() ACCEPT"));
      if (p->fn.accept)
	return p->fn.accept(p, tino_sock_acceptI(tino_sock_fdO(sock)));
      break;
    }
  tino_sock_error("tino_sockbuf_process: not handled %d", type);
  return -1;
}

static TINO_SOCKBUF
tino_sockbuf_new(int fd, const char *name, void *user)
{
  TINO_SOCK	sock;
  TINO_SOCKBUF	sb;

  cDP(("tino_sockbuf_new(%d, '%s', %p)", fd, name, user));
  sb		= tino_alloc0(sizeof *sb);
  tino_buf_init(&sb->in);
  tino_buf_init(&sb->out);
  sb->user	= user;
  sb->next	= 0;
  sb->name	= tino_strdup(name);
  if (fd<0)
    sock	= tino_sock_newAn(tino_sockbuf_process, sb);
  else
    sock	= tino_sock_new_fdAn(fd, tino_sockbuf_process, sb);
  sb->sock	= sock;
  cDP(("tino_sockbuf_new() %p", sb));
  return sb;
}

static TINO_SOCKBUF
tino_sockbuf_new_connect(const char *target, void *user)
{
  cDP(("tino_sockbuf_new_connect('%s', %p)", target, user));
  return tino_sockbuf_new(tino_sock_tcp_connect(target, NULL), target, user);
}

static TINO_SOCKBUF
tino_sockbuf_new_listen(const char *bind, void *user)
{
  cDP(("tino_sockbuf_new_listen('%s', %p)", bind, user));
  return tino_sockbuf_new(tino_sock_tcp_listen(bind), bind, user);
}

#if 0
static TINO_SOCKBUF
tino_sockbuf_new_gen(const char *def, void *user)
{
  int	fd;

  fd	= tino_sock_gen(def);
  return tino_sockbuf_new(fd, def, user);
}
#endif

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

static const char *
tino_sockbuf_name(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("tino_sockbuf_user(%p) %s", buf, buf->name));
  return buf->name;
}

static TINO_SOCK
tino_sockbuf_get(TINO_SOCKBUF buf)
{
  return buf->sock;
}

static int
tino_sockbuf_fd(TINO_SOCKBUF buf)
{
  return tino_sock_fdO(buf->sock);
}


#undef	cDP
#endif
