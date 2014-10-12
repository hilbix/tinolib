/* Additional DATA processing tools
 *
 * Copyright (C)2009-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 */

#ifndef tino_INC_data_tool_h
#define tino_INC_data_tool_h

#include "data.h"
#include "aux.h"

/* Write my standard timestamp to the stream
 *
 * Flags can be:
 * 0	to just write a second exact timestamp
 * 1	to inclure microseconds in timestamp
 * 2	to include pid_t of running process int timestamp
 * 3	to include both, pid and microseconds
 *
 * I really need my version of printf written, with this it could be
 * expressed in ONE SINGLE LINE, sigh.
 */
static void
tino_data_timestampO(TINO_DATA *d, int flags)
{
  char	tmp[65];

  switch (flags)
    {
    case 0:
    case 1:
      tino_timestampOn(tmp, sizeof tmp, flags, NULL);
      break;

    default:
      tino_stamp65Ons(tmp, flags&1);
      break;
    }
  tino_data_putsA(d, tmp);
}

#endif
