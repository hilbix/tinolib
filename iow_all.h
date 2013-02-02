/* $Header$
 *
 * NOT YET IMPLEMENTED
 * UNIT TEST FAILS *
 *
 * IO wrapper: Wrapper to "all" other wrappers
 *
 * Copyright (C)2005-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.2  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.1  2007-08-06 15:55:39  tino
 * make test now works as expected
 */

#ifndef tino_INC_iow_all_h
#define tino_INC_iow_all_h

#include "iow.h"
#ifndef TINO_IOW_ALL_NOINCLUDE
#include "iow_socket.h"
#endif
#include "str.h"

// zip+ssl+socket:host:port
static TINO_IOW
tino_iow_all(TINO_IOW w, const char *what)
{
  static struct tino_iow_link *tino_iow_all_links[] =
    {
      TINO_IOW_SOCKET_LINK,
    };

  for (i=0; i<sizeof tino_iow_all_links; i++)
    {
      struct tino_iow_link	*ptr;

      for (ptr=tino_iow_all_links[i]; ptr->name; ptr++)
	{
	  const char	*tmp;

	  if ((tmp=tino_strprefixcmp2(what, ptr->name))!=0)
	    if (*ptr++=='(')
	      {
	      }
	      if (*ptr++==':')
		{
		}
    }
}

#endif
