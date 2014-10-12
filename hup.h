/* Simple hangup handler
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

#ifndef tino_INC_hup_h
#define tino_INC_hup_h

#include "strprintf.h"
#include "signals.h"

#include <stdarg.h>

#define	tino_hup_global	tino_hup_2034s2sf2rujwf8jivf89239kssd

/** Use tino_hup as follows:
 *
 * tino_hup_handlerO(1, hup_handler, NULL);
 * 			-> hup_handler intercepts SIGHUP processing
 * tino_hup_startO("errormessage");
 *			-> set message to hup_handler
 *			-> implicitely calls tino_hup_ignoreO(0)
 * ...
 * tino_hup_ignoreO(1);	-> stop fireing hup_handler asynchronously
 * loop()
 *   {
 *   ...
 *   tino_hup_checkO();	-> fires hup_handler if pending
 *   ...
 *   }
 * tino_hup_stopO();
 *
 * The default handler is to abort the program.
 */

/** Internal global variables, do not rely on these variables, ever!
 */
typedef int			tino_hup_handler_fn_t(const char *, void *user);

struct tino_hup_global
  {
    char			*text;
    int				cnt;
    int				sigign;
    tino_hup_handler_fn_t	*handler;
    void			*user;
  } tino_hup_global;

/** There are two modes of operation:
 *
 * Synchronous, where you tino_hup_ignoreO(1) the HUP and regularily
 * call tino_hup_pending() yourself, or asynchronously where you use
 * tino_hup_ignoreO(0).  In the latter case tino_hup_pending()
 * automatically is fires from the builtin signal handler.
 */
static void
tino_hup_checkO(void)
{
  if (!tino_hup_global.cnt)
    return;
  if (tino_hup_global.handler && !tino_hup_global.handler(tino_hup_global.text, tino_hup_global.user))
    return;
  perror(tino_hup_global.text ? tino_hup_global.text : "SIGHUP");
  TINO_ABORT(1);
}

/** Internal routine: Call tino_hup_check() if SIGHUP processing is
 * set to asynchronously
 */
static void
tino_hup_check_ifO(void)
{
  if (!tino_hup_global.sigign)
    tino_hup_checkO();
}

/** Process the signal
 */
static void
tino_hup_signalO(void)
{
  tino_hup_global.cnt++;
  tino_signal(SIGHUP, tino_hup_signalO);
  tino_hup_check_ifO();
}

/** This initializes HUP processing.
 *
 * You can call this with ign=-1 in case you want to re-initialize the
 * signal processing for some reason.
 *
 * For lazy people like me forgetting about details this is
 * implicitely called from tino_hup_start() and tino_hup_handler().
 */
static void
tino_hup_ignoreO(int ign)
{
  if (ign>=0)
    tino_hup_global.sigign	= ign;
  tino_signal(SIGHUP, tino_hup_signalO);
  tino_sigfix(SIGHUP);
  tino_hup_check_ifO();
}

/** Start HUP processing: Set it to asynchronously and remember a
 * message to the handler.
 *
 * The default action is to terminate the program with the given
 * message.  If a tino_hup_handler() is set, this can intercept the
 * message, too.
 *
 * The text message is saved in an allocated buffer.  You can set this
 * to an empty string, but you cannot NULL it anymore, as NULL means
 * "just start processing".
 */
static void
tino_hup_startO(const char *s, ...)
{
  tino_va_list	list;

  if (s)
    {
      if (tino_hup_global.text)
	tino_freeO(tino_hup_global.text);
      tino_va_start(list, s);
      tino_hup_global.text	= tino_str_vprintf(&list);
      tino_va_end(list);
    }
  tino_hup_ignoreO(0);
}

/* Stop the HUP processing
 *
 * This just disables the signal but does not change internal things
 * (it does not even clear the message from tino_hup_start()), such
 * that you can re-enable it again with tino_hup_ignoreO(-1).
 */
static void
tino_hup_stopO(void)
{
  tino_sigign(SIGHUP);
}

/** Set the handler to handle HUPs.  If the handler returns 0 the
 * default action (program terminaton) is not taken.
 *
 * Note that there is a short race condition where the user pointer is
 * NULL.  If a signal strikes in this particuliar situation you might
 * find a handler to be called with a NULL user pointer.  To
 * circumvent this, first block the HUP: tino_hup_ignoreO(1);
 * tino_hup_handler(..); tino_hup_ignoreO(0); Often you can live with
 * this race as you can check for it in the handler.  Usually it won't
 * matter as you will use a NULL user pointer anyway.
 */
static void
tino_hup_handlerOns(int mode, tino_hup_handler_fn_t *fn, void *user)
{
  tino_hup_global.user		= 0;	/* race start	*/
  tino_hup_global.handler	= fn;
  tino_hup_global.user		= user;	/* race end	*/
  if (mode>=0)
    tino_hup_ignoreO(mode);
}

#undef tino_hup_global
#endif
