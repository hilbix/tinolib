/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * Terminal/Screen movement
 *
 * ADD_LDFLAGS	-ltermcap|-lcurses
 *
 * Whatever they did with curses/termcap is an awful and big cruelty.
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.2  2007-01-25 04:39:15  tino
 * Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
 *
 * Revision 1.1  2006/08/23 00:51:12  tino
 * see comment in files (term.h new)
 *
 */

#ifndef tino_INC_term_h
#define tino_INC_term_h

#include "alloc.h"

/* Define away all the quirky stuff
 *
 * If you use some names, please make sure it does not collide with
 * common names people might use ..
 */
#define	CUR	crap_defines_0
#define	lines	crap_defines_1

#include <curses.h>
#include <term.h>

/* SIGH	*/
#undef	lines
#undef	CUR

static struct tino_term
  {
  };
static int		tino_term_width, tino_term_height;
static const char	*tino_term_cm;

/* See also http://www.delorie.com/gnu/docs/termcap/
 */
static void
tino_term_flush(void)
{
  fflush(stdout);
}

static void
tino_term_out_n(const char *s, int n)
{
  fwrite(s, n, 1, stdout);
}

static void
tino_term_out_c(unsigned char c)
{
  putchar(c);
}

static void
tino_term_out(const char *s)
{
  fputs(s, stdout);
}

static void
tino_term_goto(int line, int col)
{
  tino_term_out(tgoto(tino_term_cm, col, line));
}

static const char *
tino_term_get(const char *s)
{
  char	tc[3];

  tc[0]	= s[0];
  tc[1]	= s[1];
  tc[2]	= 0;
  return tgetstr(tc, NULL);
}

static int
tino_term_cap(const char *s)
{
  const char *c;

  c	= tino_term_get(s);
  if (!c)
    return 0;
  tino_term_out(c);
  return 1;
}

static void
tino_term_clr(void)
{
  if (tino_term_cap("cl"))
    return;
  tino_term_goto(0,0);
}

static void
tino_term_inverse(void)
{
  tino_term_cap("mr");
  tino_term_cap("mh");
}

static void
tino_term_underline(void)
{
  if (tino_term_cap("us"))
    tino_term_cap("md");
}

static void
tino_term_normal(void)
{
  tino_term_cap("me");
}

static void
tino_term_scroll_up(int x, int y, int w, int h)
{
}

static int
tino_term_init(void)
{
  static char	term_buffer[BUFSIZ];
  char		*term;

  term  = getenv ("TERM");
  if (!term ||
      tgetent(term_buffer, term)<=0)
    return 1;

  tino_term_width	= tgetnum("co");
  tino_term_height	= tgetnum("li");
  if (tino_term_width<0 || tino_term_height<0)
    return 1;

  tino_term_cm		= tino_strdup(tino_term_get("cm"));
  return 0;
}

static void
tino_term_end(void)
{
  if (tino_term_cap("ll"))
    tino_term_goto(tino_term_height-1, 0);
  tino_term_out("\n");
  free((char *)tino_term_cm);
  tino_term_cm	= 0;
}

#endif
