/* $Header$
 *
 * Signal handling
 * 
 * Copyright (C)2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.2  2007-04-16 19:52:21  tino
 * See ChangeLog
 *
 * Revision 1.1  2007/04/10 10:56:46  tino
 * Better signal handling using new signals.h
 *
 */

#ifndef tino_INC_signals_h
#define tino_INC_signals_h

#include <signal.h>
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
  if (signal(sig, (void *)fn)==SIG_ERR)
    tino_fatal("signal");
}

/** Convenience function to call to siginterrupt
 */
static void
tino_siginterrupt(int sig, int flag)
{
  if (siginterrupt(sig, flag))
    tino_fatal("siginterrupt");
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
  sa.sa_handler	= (void *)fn;

  if (sigaction(sig, &sa, NULL))
    tino_fatal("sigaction");
  tino_sigfix(sig);
}

static void
tino_sigsuspend(void)
{
  sigset_t	sigs;

  sigemptyset(&sigs);
  sigsuspend(&sigs);
}

#endif
