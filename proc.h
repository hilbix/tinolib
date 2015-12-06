/* Process and thread handling.
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_proc_h
#define tino_INC_proc_h

#include "file.h"
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
	if ((unsigned)*p>pos && (unsigned)*p<max)
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
 * Returns the maximum FD seen
 *
 * WARNING: The list is altered!
 *
 * I flipped the args to show that n is the count of fds.
 */
static int
tino_fd_move(int *fds, int cnt)
{
  int	open[TINO_OPEN_MAX];
  int	i, max;

  cDP(("(%p,%d)", fds, cnt));
  /* Count the references to an fd.
   */
  TINO_FATAL_IF(cnt>=TINO_OPEN_MAX);
  memset(open, 0, sizeof open);
  max	= 0;
  for (i=cnt; --i>=0; )
    {
      cDP(("() fd%d=%d", i, fds[i]));
      if (fds[i]>=0)
	{
	  TINO_FATAL_IF(fds[i]>=TINO_OPEN_MAX);
	  open[fds[i]]++;
	}
      if (fds[i]>max)
	max	= fds[i];
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
		cDP(("() dup %d to %d", fds[i], i));
		dup2(fds[i], i);
		open[i]++;
		if (!--open[fds[i]])
		  {
		    ok	= 1;
		    cDP(("() close %d", fds[i]));
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
      cDP(("() dup %d to %d", conflict, ok));
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
  return max;
}

/* fork a program with filehandles redirected to the given fds[] (of cnt length)
 * argv[0] is the program to execute
 * if env!=NULL then the environment is changed, if addenv is not set, environment is replaced.
 *
 * There is a special treatment for the usual first 3 FDs (0,1,2), as
 * those are not closed automatically.
 *
 * keepfd is the list of file descriptors to keep: this is, all FDs
 * from 3 to up to the highest FD in this list are closed if they are
 * not in the list.  The last fd in the list must be <=0
 */
static pid_t
tino_fork_execE(int *fds, int cnt, char * const *argv, char * const *env, int addenv, int *keepfd)
{
  pid_t	chld;

  cDP(("(%p(%d)[%d,%d,%d], %p, %p, %d)", fds,cnt,fds[0],fds[1],fds[2], argv, env, addenv));
  if ((chld=fork())==0)
    {
      tino_fd_move(fds, cnt);
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
      tino_exit_n(127, "execvp(%s)", *argv);
    }
  cDP(("() chl=%d", (int)chld));
  return chld;
}

static pid_t
tino_fork_execO(int *fds, int cnt, char * const *argv, char * const *env, int addenv, int *keepfd)
{
  pid_t	chld;
  
  chld = tino_fork_execE(fds, cnt, argv, env, addenv, keepfd);
  if (chld==(pid_t)-1)
    tino_exit("fork");
  return chld;
}

/* Old convenience routine
 */
static pid_t
tino_fork_exec(int i,int o,int e, char * const *argv, char * const *env, int addenv, int *keepfd)
{
  int	fd[3];

  fd[0]=i;
  fd[1]=o;
  fd[2]=e;
  return tino_fork_execO(fd,3,argv,env,addenv,keepfd);
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
tino_wait_child_p(pid_t *child, long timeout, int *status)
{
  time_t	now;
  pid_t		pid;
  long		delta;

  cDP(("(%p(%d), %ld, %p)", child, child ? *child : 0, timeout, status));
  if (timeout>0)
    time(&now);
  delta	= timeout;
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
	  if (!child || pid==*child)
	    break;
	  if (*child==0)
	    {
	      *child	= pid;
	      break;
	    }
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

static int
tino_wait_child(pid_t child, long timeout, int *status)
{
  return tino_wait_child_p(&child, timeout, status);
}

/*
 * Returns:
 * 0..255 for the exit status
 * -1 for signal
 * -2 for core
 * -3 should not occur
 *
 * Fills cause with a human readable status string if it is not NULL
 */
static int
tino_wait_child_status(int status, char **cause)
{
  int	core;
  char	*str;

  str	= "exited";
  core	= 0;
#ifdef WCOREDUMP
  if (WCOREDUMP(status))
    {
      str	= "dumped core";
      core	= 1;
    }
#endif
  if (WIFEXITED(status))
    {
      if (cause)
	*cause	= tino_str_printf("%s with status %d", str, WEXITSTATUS(status));
      return WEXITSTATUS(status);
    }
  if (WIFSIGNALED(status))
    {
      if (cause)
	*cause	= tino_str_printf("%s with signal %d", str, WTERMSIG(status));
      return -1;
    }
  if (core)
    {
      if (cause)
	*cause	= tino_str_printf("%s (unknown cause)", str);
      return -2;
    }
  if (cause)
    *cause	= tino_str_printf("%s by unknown cause", str);
  return -3;
}

/* return the status string of the wait status.
 * This buffer must be freed!
 */
static char *
tino_wait_child_status_string(int status, int *result)
{
  char	*cause;
  int	ret;

  ret	= tino_wait_child_status(status, &cause);
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
tino_wait_child_exact(pid_t child, char **cause)
{
  int	status;

  /* This can only return 0
   */
  tino_wait_child(child, -1l, &status);
  return tino_wait_child_status(status, cause);
}

/* Poll for any child
 *
 * Returns:
 * 0	nothing happened
 * pid	the PID returned
 *
 * Status is set to:
 * 0..255 the exit status
 * -1 for signal
 * -2 for core
 * -3 should not occur
 */
static pid_t
tino_wait_child_poll(int *stat, char **cause)
{
  int	status, ret;
  pid_t	child;

  child	= 0;
  if (tino_wait_child_p(&child, 0l, &status))
    return 0;	/* t_w_c_p returned 1	*/
  ret	= tino_wait_child_status(status, cause);
  if (stat)
    *stat	= ret;
  return child;
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
      if (!(fdflags&(1<<0)))
	dup2(fd, 0);
      if (!(fdflags&(1<<1)))
	dup2(fd, 1);
      if (!(fdflags&(1<<2)))
	dup2(fd, 2);
      close(fd);
    }

  ret	= setsid();
  cDP(("() child setsid()=%d", ret));
  ret	= chdir("/");
  cDP(("() child chdir(\"/\")=%d", ret));
  IGUR(ret);
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

static pid_t
tino_forkA(void)
{
  pid_t	pid;

  if ((pid=fork())<0)
    tino_exit("fork");
  return pid;
}

#undef	cDP
#endif
