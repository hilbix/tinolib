/* Additional socket helpers, these need file.h
 *
 * Copyright (C)2006-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_sock_tool_h
#define tino_INC_sock_tool_h

#include "file.h"
#include "sock.h"
#include "alloc.h"

/** File descriptor wrapper for blocking/nonselectable sockets
 *
 * read from fd (rw=0) or write to fd (rw=1) via a fork()ed loop.
 *
 * Returns the socket.
 */
static int
tino_sock_wrapO(int fd, int rw)
{
  pid_t	p;
  int	socks[2];
  int	tmp;

  tino_sock_pairA(socks);
  if ((p=TINO_F_fork())==0)
    {
      char	buf[BUFSIZ];
      int	got, fdr, fdw, i;

      alarm(0);
#ifdef TINO_ALARM_RUN
#endif
      i	= TINO_F_sysconf(_SC_OPEN_MAX);
      if (i<0)
        tino_sock_error("sysconf(_SC_OPEN_MAX) did not work");
      fdr	= fd;
      fdw	= socks[1];
      while (--i>=0)
	if (i!=fdr && i!=fdw)
	  tino_file_closeE(i);
      if (rw)
	{
	  fdr	= fdw;
	  fdw	= fd;
	}
      tino_file_blockE(fdr);
      tino_file_blockE(fdw);
      while ((got=tino_file_readE(fdr, buf, sizeof buf))>0)
	if (tino_file_write_allE(fdw, buf, got)!=got)
	  exit(2);
      exit(got ? 1 : 0);
    }
  if (p==(pid_t)-1)
    tino_sock_error("fork()");
  tino_file_closeE(socks[1]);

  tmp	= tino_file_nullE();
  tino_file_dup2E(tmp, fd);
  tino_file_closeE(tmp);

  return socks[0];
}

/** Return an allocated buffer which returns the hostname
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
  return tino_strdupO(name);
}

/** Kill the socket if it is a unix one
 */
static void
tino_sock_unix_kill_ifO(const char *name)
{
  if (!strchr(name, ':') && !tino_file_notsocketE(name))
    tino_file_unlinkO(name);
}

#endif
