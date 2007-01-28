/* $Header$
 *
 * SLIST binding for arrays
 *
 * Copyright (C)2006 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.1  2007-01-28 03:02:07  tino
 * See ChangeLog 2006-12-16
 *
 */

#ifndef tino_INC_array_slist_h
#define tino_INC_array_slist_h

#include "slist.h"
#include "array.h"

/* append slist to an array
 */
static void
tino_array_add_slist(TINO_ARRAY a, TINO_SLIST list)
{
  TINO_GLIST_ENT	ent;

  for (ent=tino_glist_first(list); ent; ent=tino_glist_next(ent))
    {
      tino_array_add_p(a, tino_glist_data(ent));
    }
}

#endif
