/* $Header$
 *
 * Rotateable logfiles
 *
 * Copyright (C)2006-2007 by Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.7  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.6  2007-12-22 10:00:49  tino
 * Bugfix
 *
 * Revision 1.5  2007-10-04 13:00:54  tino
 * Cleanups and more functions
 *
 * Revision 1.4  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.3  2007/03/25 23:21:10  tino
 * See ChangeLog 2007-03-26
 *
 * Revision 1.2  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 *
 * Revision 1.1  2006/10/21 01:42:42  tino
 * Added
 */

#ifndef tino_INC_log_h
#define tino_INC_log_h

#include "filetool.h"
#include "ex.h"

#include <time.h>

static const char	*tino_log_filename;

/* do not call this before the last fork
 */
static void
tino_log_vprintfO(const char *prefix, int err, TINO_VA_LIST list)
{
  FILE		*fd;
  struct tm	tm;
  time_t	tim;
  static pid_t	pid;

  fd	= stderr;
  if (!tino_log_filename || (*tino_log_filename && (fd=fopen(tino_log_filename, "a+"))==0))
    return;

  time(&tim);
  gmtime_r(&tim, &tm);
  if (!pid)
    pid	= getpid();
  fprintf(fd,
	  "%4d-%02d-%02d %02d:%02d:%02d %ld: ",
	  1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
	  tm.tm_hour, tm.tm_min, tm.tm_sec,
	  (long)pid);
  tino_vfprintf(fd, list);
  fputc('\n', fd);
  if (fd==stderr)
    fflush(fd);
  else
    fclose(fd);
  return;
}

static void
tino_logO(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_log_vprintfO(NULL, 0, &list);
  tino_va_end(list);
}

static void
tino_log_errorO(const char *prefix, TINO_VA_LIST list, int err)
{
  tino_verror_std(prefix, list, err);
  tino_log_vprintfO(prefix, err, list);
}

static void
tino_log_fileO(const char *name)
{
  if (tino_log_filename && *tino_log_filename)
    tino_free_constO(tino_log_filename);
  tino_log_filename	= 0;
  if (name)
    tino_log_filename	= (!*name || !strcmp(name, "-")) ? "" : tino_file_realpath(name);
}

#endif
