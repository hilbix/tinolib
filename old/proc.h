/* $Header$
 *
 * Process and thread handling.
 *
 * Copyright (C)2005 by Valentin Hilbig
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.4  2005-08-19 04:26:39  tino
 * release socklinger 1.3.0
 *
 * Revision 1.3  2005/06/22 21:14:26  tino
 * better delta calculation
 *
 * Revision 1.2  2005/06/04 14:07:35  tino
 * typo fix
 *
 * Revision 1.1  2005/04/10 00:36:22  tino
 * TINO_FATAL_IF
 */

#ifndef tino_INC_proc_h
#define tino_INC_proc_h

#include "fatal.h"
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TINO_OPEN_MAX	1024

#if 0
/* Moves FD such, that it's not in the range [0..min[
 * If it happens to become n it is immediately used.
 *
 * This is for such type of usage, where you permutate stdin, stdout
 * and stderr.
 *
 * THIS IS INEFFICIENT
 */
static void
tino_fd_safe(int n, int min, int *fd)
{
  int	*b	= alloca((min+1)*sizeof *n);

  if (!fd || *fd<0 || *fd==n || *fd>=min)
    return;
  for (i=0; (b[i]=dup(*fd))<min && b[i]!=n; )
    {
      if (b[i]<0)
	TINO_EXIT(("dup"));
      if (++i>min)
	TINO_FATAL(("dup gave fd %d after %d iterations", b[i], i));
    }
  close(*fd);
  *fd	= b[i];
  while (--i>=0)
    if (close(b[i]))
      TINO_FATAL("close");
}
#endif

/* Move FDs to a new location.
 * The list is organized as fd[to]=from;
 * WARNING: The list is altered!
 */
static void
tino_fd_move(int n, int *fds)
{
  int	open[TINO_OPEN_MAX];
  int	i;

  /* Count the references to an fd.
   */
  memset(open, 0, sizeof open);
  for (i=n; --i>=0; i++)
    if (fds[i]>=0)
      {
	TINO_FATAL_IF(fds[i]>=TINO_OPEN_MAX);
	open[fds[i]]++;
      }

  /* Now loop until all conflicts are solved
   */
  for (;;)
    {
      int	ok, conflict;

      /* DUP all the FDs which do not overwrite sources
       */
      ok	= 0;
      conflict	= -1;
      for (i=n; --i>=0; )
	if (fds[i]>=0 && fds[i]!=i)
	  {
	    if (open[i])
	      conflict	= i;
	    else
	      {
		dup2(fds[i], i);
		open[i]++;
		if (!--open[fds[i]])
		  {
		    ok	= 1;
		    close(fds[i]);
		  }
		fds[i]	= i;
	      }
	  }
      /* we have closed something,
       * so reloop.
       */
      if (ok)
	continue;
      if (conflict<0)
	break;

      /* We have a conflict position
       * dup it anywhere else.
       * This *must* free the conflict,
       * as we already have moved any FDs we can move.
       * So the dupped fd can be
       * neither a source nor a destination fd.
       */
      ok	= dup(conflict);
      if (ok<0)
	TINO_FATAL(("cannot dup conflicting fd %d", conflict));
      TINO_FATAL_IF(ok>=TINO_OPEN_MAX);
      TINO_FATAL_IF(open[ok]);
      for (i=n; --i>=0; )
	if (fds[i]==conflict)
	  {
	    fds[i]	= ok;
	    open[ok]++;
	    open[conflict]--;
	  }
      TINO_FATAL_IF(open[conflict]);
      /* close(conflict);
       * not needed as it is a destination of dup2() in the next loop.
       */
    }
}

static pid_t
tino_fork_exec(int stdin, int stdout, int stderr, char * const *argv)
{
  pid_t	chld;

  if ((chld=fork())==0)
    {
      int	fd[3];

      fd[0]	= stdin;
      fd[1]	= stdout;
      fd[2]	= stderr;
      tino_fd_move(3, fd);

      execvp(*argv, argv);
      tino_exit("execvp(%s)", *argv);
    }

  if (chld==(pid_t)-1)
    tino_exit("fork");
  return chld;
}

/* well, this is funny
 * no convenience here anywhere else, please?
 *
 * timeout is the maximum time to wait.
 * timeout=-1 means wait forever.
 * timeout=0 means don't wait at all.
 *
 * Sideeffects: uses alarm
 *
 * Returns:
 * -1	timeout
 * 0	something exited.  If child!=0 then it's the child waited for.
 * 1	timeout==0 and nothing exited (poll unsuccessful).
 */
static int
tino_wait_child(pid_t child, long timeout, int *status)
{
  time_t	now;
  pid_t		pid;
  long		delta;

  if (timeout>0)
    {
      time(&now);
      delta	= timeout;
    }
  for (;;)
    {
      if (timeout>0)
	{
	  if (delta>10000)
	    delta	= 10000;
	  alarm(delta);
	}
      pid	= waitpid((pid_t)-1, status, (!timeout ? WNOHANG : 0));
      alarm(0);
      if (!pid)
	return 1;
      if (pid!=(pid_t)-1)
	{
	  if (child==0 || pid==child)
	    break;
	}
      else
	{
	  if (errno!=EINTR)
	    TINO_EXIT(("wait"));
	}
      if (timeout>0)
	{
	  time_t	then;

	  time(&then);
	  if ((delta=(long)(then-now))>=timeout)
	    return -1;
	  delta	= timeout-delta;
	}
    }
  return 0;
}

#endif
