/* $Header$
 *
 * NOT YET IMPLEMENTED
 * UNIT TEST FAILS *
 *
 * IO wrapper for sockets
 * Copyright (C)2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 * 
 * This is a simple IO wrapper.  The idea is, to have one single
 * interface to open/read/write/close for any operation needed, such
 * that the access can be abstract to the calling function.
 *
 * Also there is a generic easy error handler, which processes all
 * errors.
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
 * Revision 1.1  2007-08-06 15:55:39  tino
 * make test now works as expected
 *
 * Revision 1.1  2005/06/28 20:10:28  tino
 * started to add IOW (IO wrapper)
 *
 */

#ifndef tino_INC_iow_socket_h
#define tino_INC_iow_socket_h

#include "iow.h"
#include "sock.h"
#include "threads.h"

/* this is not reentrant
 */
static TINO_IOW tino_iow_socket_error_param;

static void
tino_iow_socket_error(const char *s, va_list list)
{
  TINO_IOW tmp;

  tmp				= tino_iow_socket_error_param;
  tino_sock_error_unlock();
  tino_iow_verror(tmp, s, list);
}

static int
tino_iow_socket_open(TINO_IOW w, const char *what)
{
  tino_iow_set_name(w, what);

  tino_sock_error_lock(tino_iow_socket_error);
  tino_iow_socket_error_param	= w;
  w->fd	= tino_sock_tcp_connect(what, NULL);
  tino_sock_error_unlock();
  return w->fd;
}

static TINO_IOW
tino_iow_socket(TINO_IOW w, void *user)
{
  w		= tino_iow_init(w, user);
  w->open	= tino_iow_socket_open;
  return w;
}

#endif
