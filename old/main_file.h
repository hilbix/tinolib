/* $Header$
 *
 * Standard type main programs: Standard file targets without getopt
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.3  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.2  2007-09-17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.1  2007/01/25 04:40:49  tino
 * Improvements in getopt and standard "main" routines (error-behavior).
 * getopt not yet completely ready, commit because this here works again (mostly).
 *
 * Revision 1.2  2006/10/04 02:29:10  tino
 * More tino_va_*
 */

#ifndef tino_INC_main_file_h
#define tino_INC_main_file_h

#include "main.h"

/* Simple file driven main program
 *
 * Reads a list of files from commandline or stdin
 *
 * Prototype has changed:
 * fn moved to the beginning, errflag introduced (use NULL)
 */
static int
tino_main_file(void (*fn)(const char *),
	       int *errflag,
	       int argc, char **argv,
	       const char *usage)
{
  TINO_BUF	buf;
  int		i;
  int		err;

  tino_main_set_error(&err, errflag);
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

  tino_buf_initO(&buf);
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
  return tino_main_get_error(err);
}

#endif
