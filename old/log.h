/* $Header$
 *
 * Rotateable logfiles
 *
 * Copyright (C)2006 by Valentin Hilbig
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
 * Revision 1.1  2006-10-21 01:42:42  tino
 * Added
 *
 */

#ifndef tino_INC_log_h
#define tino_INC_log_h

#include "filetool.h"
#include "ex.h"

static char	*tino_log_filename;

/* do not call this before the last fork
 */
static void
tino_log_vprintf(const char *prefix, int err, const char *s, TINO_VA_LIST list)
{
  FILE		*fd;
  struct tm	tm;
  time_t	tim;
  static pid_t	pid;

  fd	= stderr;
  if (!tino_log_filename ||
      (strcmp(tino_log_filename, "-") && (fd=fopen(tino_log_filename, "a+"))==0))
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
  tino_vfprintf(fd, s, list);
  fputc('\n', fd);
  if (fd==stderr)
    fflush(fd);
  else
    fclose(fd);
  return;
}

static void
tino_log(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_log_vprintf(NULL, 0, s, &list);
  tino_va_end(list);
}

static void
tino_log_error(const char *prefix, const char *s, TINO_VA_LIST list, int err)
{
  tino_va_list	list2;

  tino_verror_std(prefix, s, list, err);
  tino_log_vprintf(prefix, err, s, &list2);
}

static void
tino_log_file(const char *name)
{
  if (tino_log_filename)
    free(tino_log_filename);
  tino_log_filename	= 0;
  if (name)
    tino_log_filename	= tino_file_realpath(NULL, 0, name);
}

#endif