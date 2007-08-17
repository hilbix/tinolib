/* $Header$
 *
 * Additional socket helpers, these need file.h
 *
 * Copyright (C)2006-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.3  2007-08-17 20:26:03  tino
 * -
 *
 * Revision 1.2  2007/08/17 18:26:21  tino
 * See ChangeLog
 *
 * Revision 1.1  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 *
 */

#ifndef tino_INC_sock_tool_h
#define tino_INC_sock_tool_h

#include "file.h"
#include "sock.h"
#include "alloc.h"

/** Wrap stdin/stdout (blocking, nonselectable) sockets
 */
static int
tino_sock_wrap(int fd)
{
  pid_t	p;
  int	socks[2];

  tino_sock_new_pair(socks);
  if ((p=TINO_F_fork())==0)
    {
      char	buf[BUFSIZ];
      int	got, fdo=socks[2];
      int	i;

      alarm(0);
      i	= TINO_F_sysconf(_SC_OPEN_MAX);
      if (i<0)
        tino_sock_error("sysconf(_SC_OPEN_MAX) did not work");
      while (--i>=0)
	if (i!=fd && i!=fdo)
	  tino_file_close(i);
      while ((got=tino_file_read(fd, buf, sizeof buf))>0)
	if (tino_file_write_all(fdo, buf, got)!=got)
	  exit(2);
      exit(got ? 1 : 0);
    }
  tino_file_close(socks[1]);
  tino_file_dup2(fd, tino_file_null());
  return socks[2];
}

/* Return an allocated buffer which returns the hostname
 */
static char *
tino_gethostname(void)
{
  char	name[256];

  if (TINO_F_gethostname(name, sizeof name))
    {
      tino_sock_error("gethostname error");
      return 0;
    }
  return tino_strdup(name);
}


#endif
