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
 * Revision 1.13  2007-09-21 18:49:30  tino
 * daemonize function
 *
 * Revision 1.12  2006/12/12 11:30:43  tino
 * tino_wait_child_status_string and CygWin improvements
 *
 * Revision 1.11  2006/10/04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 * Revision 1.10  2006/08/16 00:25:26  tino
 * One star too many
 *
 * Revision 1.9  2006/08/11 21:56:22  tino
 * tino_wait_child_exact
 *
 * Revision 1.8  2006/07/22 17:24:26  tino
 * See ChangeLog
 *
 * Revision 1.7  2006/04/11 21:22:09  tino
 * tino_fd_keep added and slight changes
 *
 * Revision 1.6  2006/02/09 11:11:50  tino
 * added close fds on fork
 *
 * Revision 1.5  2005/10/30 03:23:52  tino
 * See ChangeLog
 *
 * Revision 1.4  2005/08/19 04:26:39  tino
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
#include "debug.h"
#include "strprintf.h"

#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define	cDP	TINO_DP_proc

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

/* Close all FDs from start (including) which are not in the list of fds.
 * fds is terminated by an FD 0 or below.
 * (Usually you want to keep fd 0 to 2, so the first arg is 3 or higher.)
 *
 * The maximum FD is taken from the given list of FDs.
 *
 * DO NOT MIX THE START-FD WITH A COUNT, like in tino_fd_move()
 */
static void
tino_fd_keep(int start, int *fds)
{
  unsigned	pos;

  if (!fds)
    return;

  if (start<0)
    start	= 0;

  for (pos=start;;)
    {
      unsigned	max;
      int	*p;

      max	= (unsigned)-1;
      for (p=fds; *p>0; p++)
	if (*p>pos && *p<max)
	  max	= *p;
      if (max==(unsigned)-1)
	break;
      while (++pos<max)
	{
	  DP(("close %d", pos));
	  close(pos);
	}
    }
}

/* Move FDs to a new location.
 * The list is organized as fd[to]=from;
 * If you want to not touch an fd, use fd[to]=to!
 *
 * WARNING: The list is altered!
 *
 * I flipped the args to show that n is the count of fds.
 */
static void
tino_fd_move(int *fds, int cnt)
{
  int	open[TINO_OPEN_MAX];
  int	i;

  cDP(("tino_fd_move(%p,%d)", fds, n));
  /* Count the references to an fd.
   */
  TINO_FATAL_IF(cnt>=TINO_OPEN_MAX);
  memset(open, 0, sizeof open);
  for (i=cnt; --i>=0; )
    {
      cDP(("tino_fd_move fd%d=%d", i, fds[i]));
      if (fds[i]>=0)
	{
	  TINO_FATAL_IF(fds[i]>=TINO_OPEN_MAX);
	  open[fds[i]]++;
	}
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
      for (i=cnt; --i>=0; )
	if (fds[i]>=0 && fds[i]!=i)
	  {
	    if (open[i])
	      conflict	= i;
	    else
	      {
		cDP(("tino_fd_move dup %d to %d", fds[i], i));
		dup2(fds[i], i);
		open[i]++;
		if (!--open[fds[i]])
		  {
		    ok	= 1;
		    cDP(("tino_fd_move close %d", fds[i]));
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
       * This *must* solve the conflict,
       * as we already have moved any FDs we can move.
       * So the dupped fd can be
       * neither a source nor a destination fd.
       */
      ok	= dup(conflict);
      cDP(("tino_fd_move dup %d to %d", conflict, ok));
      if (ok<0)
	TINO_FATAL(("cannot dup conflicting fd %d", conflict));
      TINO_FATAL_IF(ok>=TINO_OPEN_MAX);
      TINO_FATAL_IF(open[ok]);
      for (i=cnt; --i>=0; )
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

/* fork a program with filehandles redirected to
 * stdin, stdout, stderr
 * argv[0] is the program to execute
 * if env!=NULL then the environment is changed, if addenv is not set, environment is replaced.
 *
 * keepfd is the list of file descriptors to keep: this is, all FDs
 * from 3 to up to the highest FD in this list are closed if they are
 * not in the list.  The last fd in the list must be <=0
 */
static pid_t
tino_fork_exec(int std_in, int std_out, int std_err, char * const *argv, char * const *env, int addenv, int *keepfd)
{
  pid_t	chld;

  cDP(("(%d-%d-%d, %p, %p, %d)", stdin, stdout, stderr, argv, env, addenv));
  if ((chld=fork())==0)
    {
      int	fd[3];

      fd[0]	= std_in;
      fd[1]	= std_out;
      fd[2]	= std_err;
      tino_fd_move(fd, 3);
      tino_fd_keep(3, keepfd);
      if (env && !addenv)
	{
	  cDP(("() child execve(%s,%p,%p)", *argv, argv, env));
	  execve(*argv, argv, env);
	}
      else
	{
	  if (addenv)
	    while (*env)
	      {
		cDP(("() child putenv: %s", *env));
		putenv(*env++);
	      }
	  cDP(("() child execvp(%s,%p)", *argv, argv));
          execvp(*argv, argv);
	}
      cDP(("() child failed"));
      tino_exit("execvp(%s)", *argv);
    }
  cDP(("() chl=%d", (int)chld));
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

  cDP(("(%d, %ld, %p)", child, timeout, status));
  if (timeout>0)
    {
      time(&now);
      delta	= timeout;
    }
  for (;;)
    {
      cDP(("() loop"));
      if (timeout>0)
	{
	  if (delta>10000)
	    delta	= 10000;
	  cDP(("() timeout=%ld", delta));
	  alarm(delta);
	}
      pid	= waitpid((pid_t)-1, status, (!timeout ? WNOHANG : 0));
      if (timeout>0)
	alarm(0);
      if (!pid)
	{
	  cDP(("() !pid"));
	  return 1;
	}
      cDP(("() pid=%d", (int)pid));
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
  cDP(("() ok"));
  return 0;
}

static char *
tino_wait_child_status_string(int status, int *result)
{
  int	core, ret;
  char	*cause;

  cause	= "exited";
  core	= 0;
#ifdef WCOREDUMP
  if (WCOREDUMP(status))
    {
      cause	= "dumped core";
      core	= 1;
    }
#endif
  if (WIFEXITED(status))
    {
      cause	= tino_str_printf("%s with status %d", cause, WEXITSTATUS(status));
      ret	= WEXITSTATUS(status);
    }
  else if (WIFSIGNALED(status))
    {
      cause	= tino_str_printf("%s with signal %d", cause, WTERMSIG(status));
      ret	= -1;
    }
  else if (core)
    {
      cause	= tino_str_printf("%s (unknown cause)", cause);
      ret	= -2;
    }
  else
    {
      cause	= tino_str_printf("%s by unknown cause", cause);
      ret	= -3;
    }
  if (result)
    *result	= ret;
  return cause;
}

/*
 * Returns:
 * 0..255 for the exit status
 * -1 for signal
 * -2 for core
 * -3 should not occur
 */
static int
tino_wait_child_exact(pid_t child, char **buf)
{
  int	status, ret;
  char	*cause;

  /* This can only return 0
   */
  tino_wait_child(child, -1, &status);

  cause	= tino_wait_child_status_string(status, &ret);
  if (buf)
    *buf	= cause;
  else
    free(cause);
  return ret;
}

/** Daemonize the program.
 *
 * Returns:
 * 0 on the child.
 * pid of child on the parent (parent shall exit).
 *
 * Redirect stdin/out/err to /dev/null.
 * Close the controlling terminal.
 * Create a new session id (session leader).
 * chdir("/") SIDEFFECT!
 *
 * All other FDs must be freed by the calling process!
 *
 * You can suppress the FD redirection by setting flags.  However be
 * sure that all links to the terminal are broken (else it stays the
 * controlling terminal!).
 */
static pid_t
tino_daemonize_pidOb(int fdflags)
{
  pid_t	pid;
  int	ret;

  cDP(("(%d)", fdflags));
  if ((pid=fork())==(pid_t)-1)
    tino_exit("cannot fork to daemonize");

  if (pid)
    {
      cDP(("() parent child=%ld", (long)pid));
      return pid;
    }

  cDP(("() child"));
  if ((fdflags&7)!=7)
    {
      int	fd;

      fd	= tino_file_nullE();
      if (!(fdflag&(1<<0)))
	dup2(fd, 0);
      if (!(fdflag&(1<<1)))
	dup2(fd, 1);
      if (!(fdflag&(1<<2)))
	dup2(fd, 2);
      close(fd);
    }

  ret	= setsid();
  cDP(("() child setsid()=%d", ret));
  ret	= chdir("/");
  cDP(("() child chdir(\"/\")=%d", ret));
  return 0;
}

/** Convenience routine:  Fully daemonize
 *
 * Continue daemonized.  Remember the Sideeffect: cd("/")
 */
static void
tino_daemonizeOb(void)
{
  cDP(("()"));
  if (tino_daemonize_pidOb(0))
    {
      cDP(("() parent exit(0)"));
      exit(0);
    }
}

#undef	cDP
#endif
