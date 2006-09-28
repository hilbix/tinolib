/* $Header$
 *
 * Standard type main programs
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
 * Revision 1.1  2006-09-28 01:54:10  tino
 * added
 *
 */

#ifndef tino_INC_main_h
#define tino_INC_main_h

#include "ex.h"
#include "buf_line.h"

static int tino_main_errflag;

static void
tino_main_verror_fn(const char *prefix, const char *s, va_list list, int err)
{
  tino_verror_std(prefix, s, list, err);
  tino_main_errflag	= 1;
}

/* Simple file driven main program
 *
 * Reads a list of files from commandline or stdin
 */
static int
tino_main_file(int argc, char **argv, void (*fn)(const char *), const char *usage)
{
  TINO_BUF	buf;
  int		i;

  tino_verror_fn	= tino_main_verror_fn;

  if (argc<2)
    {
      char	*arg0;

      arg0	= strrchr(argv[0], '/');
      if (!arg0)
	arg0	= strrchr(argv[0], '\\');
      if (!arg0)
	arg0	= argv[0];
      else
	arg0++;
      fprintf(stderr, "Usage: %s %s\n", arg0, usage);
      return 1;
    }

  tino_buf_init(&buf);
  for (i=0; ++i<argc; )
    {
      const char	*s;

      if (strcmp(argv[i], "-"))
	{
	  fn(argv[i]);
	  continue;
	}
      while ((s=tino_buf_line_read(&buf, 0, '\n'))!=0)
	fn(s);
    }
  return tino_main_errflag ? 2 : 0;
}

#endif
