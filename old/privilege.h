/* $Header$
 *
 * Functions dealing with runtime privileges (not passwd nor PAM)
 *
 * Copyright (C)2004-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.3  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.2  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 */

#ifndef tino_INC_privilege_h
#define tino_INC_privilege_h

#include <unistd.h>

/* This ends the escalated privilege.  It shall also gives up saved
 * elevation.
 *
 * I REALLY HATE THIS!  There is a call setresuid() to really drop all
 * privileges.  However it is nonstandard.  So I really only hope that
 * the call setreuid() can drop all privileges, too, on all systems.
 */
int
tino_privilege_end_elevation(void)
{
  int	ret;
  int	tmp;

  ret	= 0;
  if (getegid()!=(tmp=getgid()))
    {
      ret	|= 2;
      setregid(tmp, tmp);
    }
  if (geteuid()!=(tmp=getuid()))
    {
      ret	|= 1;
      setreuid(tmp, tmp);
    }
  return ret;
}


#endif
