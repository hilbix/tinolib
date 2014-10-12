/* Functions with implicit file error handling
 *
 * Copyright (C)2008-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

/* Same for FILE *
 */
static void
tino_file_err_fd(FILE *fd, const char *name, const char *cause, ...)
{
  tino_va_list	list;

  tino_va_start(list, cause);
  if (name)
    tino_file_err_imp(fileno(fd), name, &list, "%s", name);
  else
    tino_file_err_imp(fileno(fd), name, &list, "%d", fileno(fd));
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
tino_file_writeA(int fd, const void *buf, size_t len, const char *name)
{
  if (tino_file_write_allE(fd, buf, len)!=len)
    tino_file_err(fd, name, "cannot write %ld bytes", (long)len);
}

static int
tino_file_readA(int fd, void *buf, size_t len, const char *name)
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

static void
tino_file_seek_startA(int fd, const char *name)
{
  tino_file_size_t	len;

  len	= tino_file_lseekE(fd, (tino_file_size_t)0, SEEK_SET);
  if (len!=(tino_file_size_t)0)
    tino_file_err(fd, name, "cannot seek to file start");
}

static tino_file_size_t
tino_file_seek_endA(int fd, const char *name)
{
  tino_file_size_t	len;

  len	= tino_file_lseekE(fd, (tino_file_size_t)0, SEEK_END);
  if (len==(tino_file_size_t)-1)
    tino_file_err(fd, name, "cannot seek to file end");
  return len;
}

static void
tino_file_appendA(int fd, const void *ptr, size_t len, const char *name)
{
  tino_file_seek_endA(fd, name);
  tino_file_writeA(fd, ptr, len, name);
}

static int
tino_file_open_createA(const char *name, int flags, int mode)
{
  int	fd;

  fd	= tino_file_open_createE(name, flags, mode);
  if (fd<0)
    tino_file_err(fd, name, "cannot create file");
  return fd;
}

static int
tino_file_open_readA(const char *name)
{
  int	fd;

  fd	= tino_file_open_readE(name);
  if (fd<0)
    tino_file_err(fd, name, "cannot open file for read");
  return fd;
}

static int
tino_file_open_rwA(const char *name)
{
  int	fd;

  fd	= tino_file_open_rwE(name);
  if (fd<0)
    tino_file_err(fd, name, "cannot open file for write");
  return fd;
}

static void
tino_file_closeA(int fd, const char *name)
{
  if (tino_file_closeE(fd))
    tino_file_err(fd, name, "close error");
}

static void
tino_file_flush_fdA(int fd, const char *name)
{
  if (tino_file_flush_fdE(fd))
    tino_file_err(fd, name, "cannot sync file");
}

static void
tino_file_stat_fdA(int fd, tino_file_stat_t *st, const char *name)
{
  if (TINO_F_fstat(fd, st))
    tino_file_err(fd, name, "cannot stat file");
}

static void *
tino_file_mmapA(tino_file_size_t offset, size_t len, int prot, int flag, int fd, const char *name)
{
  void	*addr;

  if ((addr=tino_file_mmapE(NULL, len, prot, flag, fd, offset))==0)
    tino_file_err(fd, name, "cannot mmap() file");
  return addr;
}

static void
tino_file_munmapA(void *adr, size_t len, int fd, const char *name)	/* like it symmetric */
{
  if (tino_file_munmapE(adr, len))
    tino_file_err(fd, name, "cannot munmap() file");
}

/* FILE * functions
 */
static void
tino_file_fseek_ullA(FILE *fd, unsigned long long pos, const char *name)
{
  if (tino_file_fseekE(fd, (tino_file_size_t)pos, SEEK_SET))
    tino_file_err_fd(fd, name, "cannot seek to %lld", (long long)pos);
}

static void
tino_file_fseek_uA(FILE *fd, unsigned pos, const char *name)
{
  tino_file_fseek_ullA(fd, (unsigned long long)pos, name);
}

static void
tino_file_fseek_endA(FILE *fd, const char *name)
{
  if (tino_file_fseekE(fd, (tino_file_size_t)0, SEEK_END))
    tino_file_err_fd(fd, name, "cannot seek to file end");
}

static void
tino_file_fwriteA(FILE *fd, void *ptr, size_t len, const char *name)
{
  if (tino_file_fwriteE(fd, ptr, len)!=len)
    tino_file_err_fd(fd, name, "cannot write %ld bytes", (long)len);
}

static void
tino_file_fappendA(FILE *fd, void *ptr, size_t len, const char *name)
{
  tino_file_fseek_endA(fd, name);
  tino_file_fwriteA(fd, ptr, len, name);
}

#endif
