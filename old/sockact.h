/* $Header$
 *
 * High level socket actions.
 *
 * This module is far from beeing efficiently implemented.  For
 * example encapsulation/decapsulation features have a high CPU
 * overhead, as they do not use zero copy methods nor memory mapping.
 * If you cannot do it 'in-buffer' you need expensive copy operations.
 *
 * Copyright (C)2007-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
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
 * Revision 1.4  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.3  2008-05-19 09:14:00  tino
 * tino_alloc naming convention
 *
 * Revision 1.2  2007-09-17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 */

#ifndef tino_INC_sockact_h
#define tino_INC_sockact_h

#include "sockbuf.h"

/** This module creates standard socket actions based on sockbuf.
 *
 * This are things like "sockets which receive connections" or "packet
 * oriented sockets".  For example see my tool ISAR.
 *
 * This module works with status flags and callbacks.  Error handling
 * is implicite.
 */
typedef struct tino_sockact *TINO_SOCKACT;
struct tino_sockact
  {
    TINO_SOCKBUF	sock;
    void		*user;
    int			peers;
    struct tino_sockact_list
      {
	struct tino_sockact_list	*next;
	int				nr;
	TINO_SOCKACT			peer, parent;
      }			*peer, *parent;
  };

static TINO_SOCKBUF
tino_sockact_sockbufO(TINO_SOCKACT act)
{
  return act->sock;
}

static void *
tino_sockact_userO(TINO_SOCKACT act)
{
  return act->user;
}

static TINO_SOCKACT
tino_sockact_newO(int fd, const char *name, void *user)
{
  TINO_SOCKACT	act;

  act		= tino_alloc0O(sizeof *act);
  act->sock	= tino_sockbuf_newOn(fd, name, act);
  act->user	= user;
  return act;
}

static void
tino_sockact_freeOn(TINO_SOCKACT act)
{
  if (act->parent)
    {
      act->parent->next			= act->parent->parent->peer[0].next;
      act->parent->parent->peer[0].next	= act->parent;
      act->parent->peer			= 0;
      act->parent			= 0;
    }
  tino_sock_freeOns(tino_sockbuf_getO(act->sock));
}

static int
tino_sockact_addOn(TINO_SOCKACT act, TINO_SOCKACT add)
{
  struct tino_sockact_list	*tmp;
  int				nr;

  if (act->peer)
    {
      nr	= act->peers;
      (void)TINO_REALLOC0_INC(act->peer, act->peers, 2);
      if (nr<1)
	nr	= 1;
      for (; nr<act->peers; nr++)
	{
	  act->peer[nr].parent	= act;
	  act->peer[nr].nr	= nr;
	  act->peer[nr].next	= act->peer[0].next;
	  act->peer[0].next	= &act->peer[nr];
	}
    }
  tmp			= act->peer[0].next;
  act->peer[0].next	= tmp->next;
  tmp->next		= 0;
  tmp->peer		= add;
  add->parent		= tmp;
  return tmp->nr;
}

#endif
