/* $Header$
 *
 * Simple hangup handler
 *
 * Use it as follows:
 * tino_hup_start("errormessage");
 * tino_hup_handler(hup_handler);
 * ...			-> fires hup_handler if received
 * tino_hup_ignore(1);
 * ...			-> never fires hup_handler
 * tino_hup_ignore(0);	-> fires hup_handler if pending
 * tino_hup_stop();
 *
 * Copyright (C)2006 by Valentin Hilbig
 *
 * This is release early code.  Use at own risk.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.3  2006-01-29 17:50:58  tino
 * changes due to strprintf.h
 *
 * Revision 1.2  2006/01/24 22:55:26  tino
 * now it works as expected
 *
 * Revision 1.1  2006/01/24 22:41:26  tino
 * see changelog (changes for socklinger)
 */

#ifndef tino_INC_hup_h
#define tino_INC_hup_h

#include "strprintf.h"

#include <stdarg.h>
#include <signal.h>

static char	*tino_hup_text;
static int	tino_hup_cnt;
static int	tino_hup_signal_ign;
static void	(*tino_hup_handler_fn)(const char *);

static void	tino_hup_signal(int);

static void
tino_hup_ignore(int ign)
{
  if (ign>=0)
    tino_hup_signal_ign	= ign;
  signal(SIGHUP, tino_hup_signal);
  if (!tino_hup_cnt || tino_hup_signal_ign)
    return;
  if (tino_hup_handler_fn)
    tino_hup_handler_fn(tino_hup_text);
  perror(tino_hup_text ? tino_hup_text : "SIGHUP");
  TINO_ABORT(1);
}

static void
tino_hup_start(const char *s, ...)
{
  va_list	list;

  if (s)
    {
      if (tino_hup_text)
	free(tino_hup_text);
      va_start(list, s);
      tino_hup_text	= tino_str_vprintf(s, list);
      va_end(list);
    }
  tino_hup_ignore(0);
}

static void
tino_hup_stop(void)
{
  signal(SIGHUP, SIG_IGN);
}

static void
tino_hup_handler(void (*fn)(const char *))
{
  tino_hup_handler_fn	= fn;
}

static void
tino_hup_signal(int _)
{
  tino_hup_cnt++;
  tino_hup_ignore(-1);
}

#endif
