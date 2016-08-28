/* Additional socket helpers, these need file.h
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_sock_tool_h
#define tino_INC_sock_tool_h

#include "file.h"
#include "sock.h"
#include "alloc.h"

#include "signals.h"

enum TINO_SOCK_WRAP_SHAPE
  {
    TINO_SOCK_WRAP_SHAPE_NORMAL,	/* proc -> input, proc->output	*/
    /* Everything below is not yet implemented!
     */
    TINO_SOCK_WRAP_SHAPE_MONITOR,	/* monitor -> proc, monitor -> input, monitor -> output */
    TINO_SOCK_WRAP_SHAPE_INFIRST,	/* proc -> input -> output	*/
    TINO_SOCK_WRAP_SHAPE_OUTFIRST,	/* proc -> output -> input	*/
    TINO_SOCK_WRAP_SHAPE_PIPE,		/* input -> proc -> output	*/
    TINO_SOCK_WRAP_SHAPE_REVPIPE,	/* output -> proc -> input	*/
    TINO_SOCK_WRAP_SHAPE_LAST,		/* input -> output -> proc	*/
    TINO_SOCK_WRAP_SHAPE_NOCHILD,	/* proc, init -> output, init -> input	*/
    TINO_SOCK_WRAP_SHAPE_NOPROC,	/* input -> output	*/
  };

/* Copy data from FD i to FD o in the background (forked).
 *
 * child==-1: no child, does not fork and does not return.  pid_p is ignored
 * child==0: fork off a child to do the IO, return child PID in pid_p
 * child==1: become the child, *pid_p will be 0, terminate IO as soon as we or a sibling dies with error
 * child==2: as before, but ignore the error status (so do not die, but wait for children)
 * child==3: as before, but do not wait for children after IO has finished
 * else: undefined
 *
 * return 0 if forked
 * return 1 if not forked (nothing to do)
 * return -1 on error
 *
 * Child does not return and terminates with following codes:
 * 16 unspecific error (like child terminated with signal)
 * 17 write error
 * 18 read error
 * else: code of the child if a child terminates with something else than 0
 * else: 0
 */
static int
tino_sock_wrap_forkA(int i, int o, int child, pid_t *pid_p)
{
  char	buf[10*BUFSIZ];
  int	got, n;
  pid_t	pid;

  if (i<0 || o<0)
    return 1;

  n	= TINO_F_sysconf(_SC_OPEN_MAX);
  if (n<=0)
    {
      tino_sock_error("sysconf(_SC_OPEN_MAX) did not work");
      return -1;
    }

  pid	= 0;
  if (child>=0)
    {
      pid	= TINO_F_fork();
      if (pid_p)
        *pid_p = pid;
      if (pid==(pid_t)-1)
        {
          tino_sock_error("cannot fork()");
	  return -1;
        }
      if (!pid == !!child)
        return 0;
    }

  /* never return from this place	*/

  alarm(0);	/* nuke alarm thingies	*/

  /* close all file descriptors which are not needed */
  while (--n>=0)
    if (n!=i && n!=o)
      tino_file_closeE(n);

  /* XXX TODO XXX kill everything else not needed, like SHMEM etc.? */
  /* ignore most types of signals? */

  tino_sigset(SIGCHLD, child==1 ? tino_sigchld_checked : tino_sigchld_ignored);

  tino_file_blockE(i);
  tino_file_blockE(o);
  while ((got=tino_file_readE(i, buf, sizeof buf))>0)
    if (tino_file_write_allE(o, buf, got)!=got)
      exit(17);
  tino_sock_shutdownE(o, SHUT_WR);
  if (got)
    exit(18);

  if (child==3)
    wait(NULL);
  /* hopefully SIGCHLD handler fires first,
   * so exit(0) means, all children ok
   */
  exit(0);
}


/** File descriptor wrapper for blocking/nonselectable sockets
 *
 * read from first, write to second, use -1 if not needed
 * Returns the socket.
 */
static int
tino_sock_wrapO(int i, int o, enum TINO_SOCK_WRAP_SHAPE shape)
{
  int	socks[2];

  tino_FATAL(shape);	/* not yet supported	*/

  tino_sock_pairA(socks);
  tino_sock_wrap_forkA(i, socks[1], 0, NULL);
  tino_sock_wrap_forkA(socks[1], o, 0, NULL);
  tino_file_closeE(socks[1]);

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
