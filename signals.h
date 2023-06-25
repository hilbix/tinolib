/* Signal handling
 * 
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_signals_h
#define tino_INC_signals_h

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "sysfix.h"
#include "fatal.h"

#ifdef TINO_USE_NO_SIGACTION
#define	TINO_SIGNAL(SIG,FN)		tino_signal(SIG,FN)
#define	TINO_SIGACTION(SIG,FN)		tino_sigfix(SIG)
#else
#define	TINO_SIGNAL(SIG,FN)		do { ; } while (0)
#define	TINO_SIGACTION(SIG,FN)		tino_sigaction(SIG,FN)
#endif

/** The easy signal function takes no args.
 *
 * Well, it's nice to have the sig as an arg.  But implementations
 * seem to differ a little bit here, and some even may have weird
 * parameter defaults.  So here is a sane value, a pointer to a
 * completely void routine.  This shall play on all systems.
 */
typedef void (*tino_sighandler_t)(void);

/** Set a signal handler
 *
 * Note that you can use the defines above to make it more portable.
 * Then do a programming for *both* ways: sigaction() and signal().
 * This is: sigaction() is a global thing and signal() is the local
 * thing.  So you give:
 *
 * TINO_SIGACTION(sig, handler);
 * ...;
 * TINO_SIGNAL(sig, handler);
 *
 * And in the handler:
 * TINO_SIGNAL(sig, handler);
 *
 * In case of sigaction, TINO_SIGNAL just is a dummy.
 */
static void
tino_signal(int sig, tino_sighandler_t fn)
{
  if (TINO_F_signal(sig, (TINO_T_sighandler_t)fn)==SIG_ERR)
    tino_fatal("signal");
}

/** Convenience function to call to siginterrupt
 *
 * POSIX leaves it UNDEFINED what happens when certain syscalls are restarted.
 * Hence you certainly will never want this to happen on a stable system.
 *
 * But some systems are broken.  Their signals stick to restart
 * after sication() was called without(!) SA_RESTART.
 *
 * siginterrupt() is the only way to get those broken behavior fixed.
 * BUT THIS NOW GIVES AN ANNOYING DEPRECATION WARNING ON COMPILE.
 * Hence we have to use this ugly #pragma below.  Sigh.
 *
 * (I really have no idea how to differentiate between those zillion of different
 * systems out there, such to only compile the fix in when it is needed.)
 */
static void
tino_siginterrupt(int sig, int flag)
{
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  if (siginterrupt(sig, flag))
    tino_fatal("siginterrupt");
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
}

/** Set that this signal restarts syscalls
 *
 * Note that the default varies even in Linux distros and you cannot
 * be sure that syscalls are always restarted properly, as there is no
 * standard.
 */
static void
tino_sigrestart(int sig)
{
  tino_siginterrupt(sig, 0);
}

/** Set the signal to not restart syscalls
 *
 * Note that this shall be portable, as it creates a portable reaction
 * of syscalls across all Unix derivates.  However, some broken libs
 * might rely on the signal restart feature.
 */
static void
tino_sigintr(int sig)
{
  tino_siginterrupt(sig, 1);
}

/** Convenience function for one single signal
 */
static void
tino_sigprocmask_1(int how, int sig)
{
  sigset_t	sigs;

  if (sigemptyset(&sigs) ||
      sigaddset(&sigs, sig))
    tino_fatal("sigsetops");
  if (sigprocmask(how, &sigs, NULL))
    tino_fatal("sigprocmask");
}

/** Block a signal.
 *
 * Blocking a signal just ignores the signal, as if it did not happen.
 * No signal handler is invoked, just plain nothing happens.
 *
 * Well, this should work (but I never tested it) for all signals but
 * SIGKILL and SIGSTOP (this is process tracing, not ^Z).  However,
 * AFICS, not blocking signals really is more portable.
 */
static void
tino_sigblock(int sig)
{
  tino_sigprocmask_1(SIG_BLOCK, sig);
}

/** Unblock a signal.
 *
 * This shall be the normal way of dealing with a signal.  As some
 * systems spuriously block signals for new started processes (seems
 * to depend on the current tty setting), you shall make it sure that
 * your signal settings are sane.  For this there is tino_sigfix()
 * (automatically invoked by tino_sigaction).
 */
static void
tino_sigunblock(int sig)
{
  tino_sigprocmask_1(SIG_UNBLOCK, sig);
}

/** convenience routine: Set sig to SIG_IGN and block it
 */
static void
tino_sigign(int sig)
{
  tino_signal(sig, (tino_sighandler_t)SIG_IGN);
  tino_sigblock(sig);
}

/** convenience routine: unblock signal and Set sig to SIG_DFL
 */
static void
tino_sigdfl(int sig)
{
  tino_sigunblock(sig);
  tino_signal(sig, (tino_sighandler_t)SIG_DFL);
}


/** Set the signal handling to some sane default values.
 *
 * There are really some broken broken system default settings out
 * there, which even can differ while running a system.
 *
 * This unblocks the signal and makes syscalls to not restart itself
 * on this signal, such that you get the chance to process the signal
 * properly after the syscall (or interrupt waiting or whatever).
 * This is needed, as you cannot (read: shall not as this really can
 * give weird results) use most library functions in signal handlers.
 */
static void
tino_sigfix(int sig)
{
  tino_sigintr(sig);
  tino_sigunblock(sig);
}

/** Easy version of sigaction.  This here has a clean and easy to
 * understand interface and is supposed to always "just do what I
 * mean".  (I is me, not you.)
 */
static void
tino_sigaction(int sig, tino_sighandler_t fn)
{
  struct sigaction	sa;

  memset(&sa, 0, sizeof sa);
  sa.sa_handler	= (TINO_T_sighandler_t)fn;

  if (sigaction(sig, &sa, NULL))
    tino_fatal("sigaction");
  tino_sigfix(sig);
}

/** Suspend the process until any signal arrives
 *
 * Note that this currently is only tested for signals which are set
 * to a signal handler with this module.
 */
static void
tino_sigsuspend(void)
{
  sigset_t	sigs;

  sigemptyset(&sigs);
  sigsuspend(&sigs);
}

/** Internal dummy function, do not use outside
 */
static void
tino_sighandler_dummy(void)
{
  TINO_SIGNAL(sig, tino_sighandler_dummy);
}

/** Convenience routine
 */
static void
tino_sigset(int sig, tino_sighandler_t fn)
{
  TINO_SIGNAL(sig, fn);
  TINO_SIGACTION(sig, fn);
}

/** Set the signal to a dummy function
 *
 * This way make sure it is delivered to the process.
 */
static void
tino_sigdummy(int sig)
{
  tino_sigset(sig, tino_sighandler_dummy);
}

/** Convenience signal handler, ignore all children
 * Use: tino_sigset(SIGCHLD, tino_sigchld_ignored);
 */
static void
tino_sigchld_ignored(void)
{
  int   e=errno;
  while (waitpid((pid_t)-1, NULL, WNOHANG)>0);
  errno = e;
}

/** Convenience signal handler, die immediately if any child is not ok
 * Use: tino_sigset(SIGCHLD, tino_sigchld_checked);
 */
static void
tino_sigchld_checked(void)
{
  int   e=errno;
  int   status;

  while (waitpid((pid_t)-1, &status, WNOHANG)>0)
    {
      if (!WIFEXITED(status))
        exit(16);
      if (WEXITSTATUS(status))
        exit(WEXITSTATUS(status));
    }
  errno = e;
}

/** Convenience signal handler, die immediately if any child dies
 * Use: tino_sigset(SIGCHLD, tino_sigchld_fatal);
 */
static void
tino_sigchld_fatal(void)
{
  int   e=errno;
  int   status;

  while (waitpid((pid_t)-1, &status, WNOHANG)>0)
    {
      if (!WIFEXITED(status))
        exit(16);
      if (WEXITSTATUS(status))
        exit(WEXITSTATUS(status));
      exit(15);
    }
  errno = e;
}

#endif
