/* $Header$
 *
 * Set the program title which is shown in PS.
 * Note that this can also be used to hide commandline parameters.
 *
 * This is based on ideas found at different sources.
 *
 * Copyright (C)2004 by Valentin Hilbig
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
 * Revision 1.1  2004-12-07 21:45:58  tino
 * added
 *
 */

#ifndef tino_INC_setproctitle_h
#define tino_INC_setproctitle_h

#ifdef __linux__
#define TINO_SETPROCTITLE_SUPPORTED
#endif

#ifndef TINO_SETPROCTITLE_SUPPORTED

static int
tino_setproctitle_init(int argc, char **argv)
{
  return -1;
}

static int
tino_setproctitle(const char *s)
{
  return -1;
}

#else
#include "str.h"
#include "alloc.h"

/* These are internal variables, do not access these.
 * If I see someone accessing these from outside,
 * I will change their name unpredictable.
 */

static void *	tino_setproctitle_ptr;
static int	tino_setproctitle_len;

/* Initialize the proctitle string.
 * Currently works only for Linux.
 *
 * RUN THIS FIRST IN MAIN, BEFORE YOU ACCESS argv[] or environ[]!
 * This has sideeffects on argv-array.
 *
 * This also tries to hide all commandline parameters and such.
 */
static int
tino_setproctitle_init(int argc, char **argv)
{
  extern char **environ;

  char	**ptr, *start;
  int	n;

  start	= argv[0];
  n	= 0;
  for (ptr=argv; *ptr; ptr++)
    {
      xDP(("argv %p", *ptr));
      if (*ptr!=start+n)
	{
	  xDP(("arg stop"));
	  return -1;
	}
      n		+= strlen(*ptr)+1;
      *ptr	=  tino_strdup(*ptr);
    }
  xDP(("start=%p n=%d", start, n));
  for (ptr=environ; *ptr; ptr++)
    {
      xDP(("env %p", *ptr));
      if (*ptr!=start+n)
	{
	  xDP(("env stop"));
	  return -1;
	}
      n		+= strlen(*ptr)+1;
      *ptr	= tino_strdup(*ptr);
    }
  xDP(("start=%p n=%d", start, n));

  tino_setproctitle_ptr	= start;
  tino_setproctitle_len	= n;

  return n;
}

static int
tino_setproctitle(const char *s)
{
  int	i;

  if (!tino_setproctitle_ptr || tino_setproctitle_len<=0)
    return -1;

  xDP(("pos=%p n=%d s='%s'", tino_setproctitle_ptr, tino_setproctitle_len, s));
  tino_strxcpy(tino_setproctitle_ptr, s, tino_setproctitle_len);
  return tino_setproctitle_len;
}
#endif

#ifdef TINO_TEST_MAIN
#include "debug.h"

int
main(int argc, char **argv)
{
  printf("init=%d\n", tino_setproctitle_init(argc, argv));
  printf("set=%d\n", tino_setproctitle("this is A very very long and longer test"));
  sleep(20);
  printf("end\n");
  return 0;
}
#endif
#endif
