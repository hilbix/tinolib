/* $Header$
 *
 * Sockets with buffers.
 * This builds on top of tino_sock and tino_buf.
 * Shall use tino_ob later.
 *
 * Copyright (C)2004-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.16  2007-09-26 21:29:46  tino
 * make test works again
 *
 * Revision 1.15  2007/09/26 21:09:28  tino
 * Some new functions and Cygwin fixes (started).
 *
 * Revision 1.14  2007/09/18 20:08:12  tino
 * See ChangeLog
 *
 * Revision 1.13  2007/09/17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.12  2007/08/24 10:43:43  tino
 * tino_sockbuf_freeOns and function names corrected
 *
 * Revision 1.11  2007/08/19 17:02:28  tino
 * Improved debugging
 *
 * Revision 1.10  2007/08/17 18:26:21  tino
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

#include "sock_select.h"
#include "buf.h"

#define	cDP	TINO_DP_sock

typedef struct tino_sockbuf *TINO_SOCKBUF;

struct tino_sockbuf_fn
  {
    int		(*accept	)(TINO_SOCKBUF, int);	/* return 0 to close, 1 for ok, -1 for error	*/
    int		(*poll		)(TINO_SOCKBUF, int);	/* gets passed the default poll value	*/
    void	(*poll_hook	)(TINO_SOCKBUF);
    int		(*read		)(TINO_SOCKBUF);
    void	(*read_hook	)(TINO_SOCKBUF, int);
    int		(*write		)(TINO_SOCKBUF);
    void	(*write_hook	)(TINO_SOCKBUF, int);
    int		(*eof		)(TINO_SOCKBUF);
    int		(*exception	)(TINO_SOCKBUF);
    void	(*close		)(TINO_SOCKBUF);
  };

#define	TINO_SOCKBUF_SET(BUF,WHAT,FN)	do { (BUF)->fn.WHAT=(FN); } while (0)

#define TINO_SOCKBUF_ACCEPT	accept
#define TINO_SOCKBUF_POLL	poll
#define TINO_SOCKBUF_POLL_HOOK	poll_hook
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
tino_sockbuf_inO(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("(%p) %p", buf, (buf->next ? &buf->next->out : &buf->in)));
  return (buf->next ? &buf->next->out : &buf->in);
}

inline TINO_BUF *
tino_sockbuf_outO(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("(%p) %p", buf, &buf->out));
  return &buf->out;
}

/* Accept new connections from socket
 */
static int
tino_sockbuf_processN(TINO_SOCK sock, enum tino_sock_proctype type)
{
  TINO_SOCKBUF	p=tino_sock_userO(sock);

  cDP(("(%p, %d) '%s'", p, type, p->name));
  tino_FATAL(sock!=p->sock);
  switch (type)
    {
      TINO_SOCK	tmp;
      int	ret;

    default:
      tino_fatal("tino_sockbuf_process type=%d unknown", type);

    case TINO_SOCK_PROC_CLOSE:
      if (p->fn.close)
	{
	  cDP(("() fn.close %p", p->fn.close));
	  p->fn.close(p);
	}
      cDP(("() CLOSE"));
      TINO_FREE_NULL(p->name);
      tino_buf_freeO(&p->in);
      tino_buf_freeO(&p->out);
      if (p->next)
	p->next->prev	= 0;
      p->next		= 0;
      if (p->prev)
	p->prev->next	= 0;
      p->prev		= 0;
      cDP(("() TINO_SOCK_FREE"));
      return TINO_SOCK_FREE;

    case TINO_SOCK_PROC_EOF:
      cDP(("() EOF"));
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
      cDP(("() POLL"));
      if (p->fn.poll_hook)
	p->fn.poll_hook(p);
      if (p->prev && tino_sock_stateO(p->prev->sock)<0)
	ret	= TINO_SOCK_EOF;
      else if (p->fn.accept)
	ret	= TINO_SOCK_ACCEPT;
      else
	ret	=((tino_buf_get_lenO(tino_sockbuf_outO(p)) ? TINO_SOCK_WRITE : 0) |
		  (tino_buf_get_lenO(tino_sockbuf_inO(p)) ? 0 : TINO_SOCK_READ));
      return (p->fn.poll ? p->fn.poll(p,ret) : ret);

    case TINO_SOCK_PROC_READ:
      cDP(("() READ"));
      if (p->fn.read)
	return p->fn.read(p);
      ret	= tino_buf_readE(tino_sockbuf_inO(p), tino_sock_fdO(sock), -1);
      if (p->fn.read_hook)
	{
	  int	e=errno;
	  p->fn.read_hook(p, ret);
	  errno=e;
	}
      if (p->next)
	tino_sock_pollOn(p->next->sock);
      return ret;

    case TINO_SOCK_PROC_WRITE:
      cDP(("() WRITE"));
      if (p->fn.write)
	return p->fn.write(p);
      ret	= tino_buf_write_awayI(tino_sockbuf_outO(p), tino_sock_fdO(sock), -1);
      if (p->fn.write_hook)
	{
	  int	e=errno;
	  p->fn.write_hook(p, ret);
	  errno=e;
	}
      if (p->prev)
	tino_sock_pollOn(p->prev->sock);
      return ret;

    case TINO_SOCK_PROC_EXCEPTION:
      cDP(("() EXCEPTION"));
      if (p->fn.exception)
	return p->fn.exception(p);
      break;

    case TINO_SOCK_PROC_ACCEPT:
      cDP(("() ACCEPT"));
      if (p->fn.accept)
	return p->fn.accept(p, tino_sock_acceptI(tino_sock_fdO(sock)));
      break;
    }
  tino_sock_error("tino_sockbuf_process: not handled %d", type);
  return -1;
}

static TINO_SOCKBUF
tino_sockbuf_newOn(int fd, const char *name, void *user)
{
  TINO_SOCK	sock;
  TINO_SOCKBUF	sb;

  cDP(("(%d, '%s', %p)", fd, name, user));
  sb		= tino_alloc0(sizeof *sb);
  tino_buf_initO(&sb->in);
  tino_buf_initO(&sb->out);
  sb->user	= user;
  sb->next	= 0;
  if (!name)
    name	= tino_sock_get_peernameE(fd);
  sb->name	= tino_strdupN(name);
  if (fd<0)
    sock	= tino_sock_newAn(tino_sockbuf_processN, sb);
  else
    sock	= tino_sock_new_fdAn(fd, tino_sockbuf_processN, sb);
  sb->sock	= sock;
  cDP(("() %p", sb));
  return sb;
}

static TINO_SOCKBUF
tino_sockbuf_new_connectO(const char *target, void *user)
{
  cDP(("('%s', %p)", target, user));
  return tino_sockbuf_newOn(tino_sock_tcp_connect(target, NULL), target, user);
}

static TINO_SOCKBUF
tino_sockbuf_new_listenO(const char *bind, void *user)
{
  cDP(("('%s', %p)", bind, user));
  return tino_sockbuf_newOn(tino_sock_tcp_listen(bind), bind, user);
}

#if 0
static TINO_SOCKBUF
tino_sockbuf_new_genO(const char *def, void *user)
{
  int	fd;

  fd	= tino_sock_gen(def);
  return tino_sockbuf_newOn(fd, def, user);
}
#endif

static TINO_SOCKBUF
tino_sockbuf_setO(TINO_SOCKBUF buf, struct tino_sockbuf_fn *fn)
{
  cDP(("(%p, %p)", buf, fn));
  tino_FATAL(!buf);
  buf->fn	= *fn;
  return buf;
}

static void
tino_sockbuf_nextO(TINO_SOCKBUF buf, TINO_SOCKBUF next)
{
  cDP(("(%p,%p)", buf, next));
  tino_FATAL(!buf);
  if (buf->next)
    buf->next->prev	= 0;
  buf->next	= next;
  if (next)
    next->prev	= buf;
}

static void *
tino_sockbuf_userO(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("(%p) %p", buf, buf->user));
  return buf->user;
}

static const char *
tino_sockbuf_nameO(TINO_SOCKBUF buf)
{
  tino_FATAL(!buf);
  cDP(("(%p) %s", buf, buf->name));
  return buf->name;
}

static TINO_SOCK
tino_sockbuf_getO(TINO_SOCKBUF buf)
{
  return buf->sock;
}

static int
tino_sockbuf_fdO(TINO_SOCKBUF buf)
{
  return tino_sock_fdO(buf->sock);
}

static void
tino_sockbuf_freeOns(TINO_SOCKBUF buf)
{
  tino_sock_freeOns(buf->sock);
}

#undef	cDP
#endif
