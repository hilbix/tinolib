/* $Header$
 *
 * Functions with implicit file error handling
 *
 * Copyright (C)2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.2  2008-09-20 21:28:31  tino
 * locking fixed
 *
 * Revision 1.1  2008-09-20 18:04:05  tino
 * file locks
 */

#ifndef tino_INC_fileerr_h
#define tino_INC_fileerr_h

#include "file.h"
#include "ex.h"

/* User wrapped file error handler
 *
 * If it is not set, tino_file_verr is called to print the error and the program is terminated.
 * If it is set and returns, the error is ignored.
 */
static void (*tino_file_err_fn)(int fd, const char *name, TINO_VA_LIST prefix, TINO_VA_LIST cause);

static void
tino_file_verr(int fd, const char *name, TINO_VA_LIST prefix, TINO_VA_LIST cause)
{
  tino_verror_vext(prefix, cause, errno);
}

/* XXX use tino_err()
 */
static void
tino_file_vexit(int fd, const char *name, TINO_VA_LIST prefix, TINO_VA_LIST cause)
{
  tino_file_verr(fd, name, prefix, cause);
  TINO_ABORT(-1);
}

static void
tino_file_err_imp(int fd, const char *name, TINO_VA_LIST cause, const char *prefix, ...)
{
  tino_va_list	list;

  tino_va_start(list, prefix);
  (tino_file_err_fn ? tino_file_err_fn : tino_file_vexit)(fd, name, &list, cause);
  tino_va_end(list);
}

/* File error handler
 */
static void
tino_file_err(int fd, const char *name, const char *cause, ...)
{
  tino_va_list	list;

  tino_va_start(list, cause);
  if (name)
    tino_file_err_imp(fd, name, &list, "%s", name);
  else
    tino_file_err_imp(fd, name, &list, "%d", fd);
  tino_va_end(list);
}

/** Convenience routine
 *
 * Aquire lock, exit/error if something broken
 *
 * Returns 0 if locked
 */
static int
tino_file_lockA(int fd, int writelock, int block, const char *name)
{
  switch (tino_file_lockE(fd, writelock, block))
    {
    case 0:
      return 0;
    case 1:
      return 1;
    }
  tino_file_err(fd, name, "cannot lock %d,%d", writelock, block);
  return -1;
}

/** Convenience routine
 */
static int
tino_file_lock_exclusiveA(int fd, int block, const char *name)
{
  return tino_file_lockA(fd, 1, block, name);
}

/** Convenience routine
 */
static int
tino_file_lock_sharedA(int fd, int block, const char *name)
{
  return tino_file_lockA(fd, 0, block, name);
}

static void
tino_file_unlockA(int fd, const char *name)
{
  if (tino_file_unlockE(fd))
    tino_file_err(fd, name, "cannot unlock");
}

static void
tino_file_writeA(int fd, const char *buf, size_t len, const char *name)
{
  if (tino_file_write_allE(fd, buf, len)!=len)
    tino_file_err(fd, name, "cannot write %ld bytes", (long)len);
}

static int
tino_file_readA(int fd, char *buf, size_t len, const char *name)
{
  int	n;

  n	= tino_file_readE(fd, buf, len);
  if (n<0)
    tino_file_err(fd, name, "cannot read %ld bytes", (long)len);
  return n;
}

static void
tino_file_seek_ullA(int fd, unsigned long long pos, const char *name)
{
  if (tino_file_lseekE(fd, (tino_file_size_t)pos, SEEK_SET)!=pos)
    tino_file_err(fd, name, "cannot seek to %lld", (long long)pos);
}

static void
tino_file_seek_uA(int fd, unsigned pos, const char *name)
{
  tino_file_seek_ullA(fd, (unsigned long long)pos, name);
}

#endif
