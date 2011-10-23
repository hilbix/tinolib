/* $Header$
 *
 * Standard type main programs: Standard file targets without getopt
 *
 * Copyright (C)2006-2009 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.6  2011-10-23 00:28:28  tino
 * Tinolib support with options, ANSI escapes now default
 *
 * Revision 1.5  2009-07-31 22:18:00  tino
 * Unit test works now.  io.h starts to become usable, see put.h
 * Several minor fixes and addons, see ChangeLog
 *
 * Revision 1.4  2009-07-02 01:20:10  tino
 * More standard flags
 *
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

/* This are bit flags!	*/
#define	TINO_MAIN_FILE_FLAG_LF		0	/* no ANSI, no NUL	*/
#define	TINO_MAIN_FILE_FLAG_NUL		1	/* NUL instead of LF	*/
#define	TINO_MAIN_FILE_FLAG_ANSI	2	/* Implies LF		*/
#define	TINO_MAIN_FILE_FLAG_BUFFERED	4	/* Do unbuffered output	*/

/* Simple file driven main program
 *
 * Reads a list of files from commandline or stdin
 *
 * Prototype has changed:
 * fn moved to the beginning, errflag introduced (use NULL)
 *
 * Prototype changed again:
 * VERSION introduced to simplify usage and 'flags' to function.
 *
 * See the tino_main_file_*() helpers to use 'flags' properly.  To be
 * SHIT compliant in future, stick to these routines!
 */
static int
tino_main_file(void (*fn)(const char *, int flags),
	       int *errflag,
	       int argc, char **argv,
	       const char *version,
	       const char *usage)
{
  TINO_BUF	buf;
  int		i;
  int		err;
  int		inp, out;

  tino_main_set_error(&err, errflag);
  i	= 0;
  inp	= 0;
  out	= TINO_MAIN_FILE_FLAG_ANSI;
  if (argc>1 && argv[1][0]=='-' && argv[1][1] && !argv[1][2])
    {
      i++;
      switch (argv[1][1])
	{
	default:
	  fprintf(stderr, "unknown option %s", argv[1]);
	  return 1;

	case '-': inp = -1;						break;
	case '0': inp =			TINO_MAIN_FILE_FLAG_NUL;	break;
#if 0
	case 'a': inp =			TINO_MAIN_FILE_FLAG_ANSI;	break;
#endif
	case 'b':		out =	TINO_MAIN_FILE_FLAG_BUFFERED;	break;
	case 'n': inp =		out =	TINO_MAIN_FILE_FLAG_NUL;	break;
	case 'o':		out =	0;				break;
	case 'z': inp = -1;	out =	TINO_MAIN_FILE_FLAG_NUL;	break;
	}
    }
  if (argc<2+i)
    {
      char	*arg0;

      arg0	= strrchr(argv[0], '/');
      if (!arg0)
	arg0	= strrchr(argv[0], '\\');
      if (!arg0)
	arg0	= argv[0];
      else
	arg0++;
      fprintf(stderr,
	      "Usage: %s [-option] file..\n"
	      "\t\tVersion %s compiled " __DATE__ "\n"
	      "\t%s\n"
	      "	Note that there is only one single option possible.\n"
	      "	If file is '-' STDIN is read for filenames.\n"
	      "	By default normal LF terminated lines are read\n"
	      "	and ANSI escaped LF terminated lines are written.\n"
	      "	ANSI escapes can be undone within BASH using:\n"
	      "		eval unescaped=\"\\$'$escaped'\"\n"
	      "Options: (you can give only one single option)\n"
	      "	--	none-option, disables '-' for STDIN\n"
	      "	-0	read lines NUL terminated, write ANSI escaped with LF\n"
#if 0
	      "	-a	read and write ANSI escaped lines\n"
#endif
	      "	-b	buffered output, implies -o\n"
	      "	-n	read and write NUL terminated lines, no ANSI\n"
	      "	-o	read and write LF terminated lines, no ANSI\n"
	      "	-z	write NUL terminated lines, no ANSI, disables '-'\n"
	      , arg0, version, usage);
      return 1;
    }

  tino_buf_initO(&buf);
  while (++i<argc)
    {
      const char	*s;

      if (inp<0 || strcmp(argv[i], "-"))
	{
	  fn(argv[i], out);
	  continue;
	}
      while ((s=tino_buf_line_read(&buf, 0, (inp & TINO_MAIN_FILE_FLAG_NUL ? 0 : '\n')))!=0)
	{
	  if (inp & TINO_MAIN_FILE_FLAG_ANSI)
	    000;	/* ANSI unescape	*/
	  fn(s, out);
	}
    }
  return tino_main_get_error(err);
}

/* Write some arbitrary output back to the other side.  Terminates
 * when the pipe break.
 */
static void
tino_main_file_vprintf(TINO_VA_LIST list)
{
  tino_vfprintf(stdout, list);
  if (ferror(stdout))
    exit(1);
}

static void
tino_main_file_printf(const char *s, ... )
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_main_file_vprintf(&list);
  tino_va_end(list);
}

static void
tino_main_put_ansi(const char *s)
{
  for (; *s; s++)
    if (((unsigned char)*s)<33 || ((unsigned char)*s)>=127 || *s=='\'')
      printf("\\x%02x", (unsigned char)*s);
    else
      switch (*s)
	{
	case '\\':
	  putchar('\\');
	default:
	  putchar(*s);
	  break;
	}
}

static void
tino_main_file_escape(const char *s, int flags)
{
  if (flags&TINO_MAIN_FILE_FLAG_ANSI)
    tino_main_put_ansi(s);
  else
    fputs(s, stdout);
}

/* Write proper line end
 */
static void
tino_main_file_line(int flags)
{
  putchar((flags&TINO_MAIN_FILE_FLAG_NUL) ? 0 : '\n');
  if (!(flags&TINO_MAIN_FILE_FLAG_BUFFERED))
    fflush(stdout);
}

#endif
