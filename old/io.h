/* $Header$
 *
 * UNIT TEST FAILS *
 *
 * Generic IO layer.  For the moment this only includes tino/file.h
 * IF YOU WANT TO DEPEND ON THS, USE IT AS THE FIRST INCLUDE!
 *
 * In some distant future this will handle *all* IO the same way,
 * regardless if it is a file, directory, device, socket or telepathy.
 * YKWIM.
 *
 * Please read io.txt for more information.
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.6  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.5  2005/09/10 12:31:39  tino
 * cosmetic checkin
 *
 * Revision 1.4  2005/06/04 14:35:06  tino
 * Unit test improved
 *
 * Revision 1.3  2005/03/05 19:42:54  tino
 * tino_file_mmap_anon added
 *
 * Revision 1.2  2005/01/04 13:23:49  tino
 * see ChangeLog, mainly changes for "make test"
 *
 * Revision 1.1  2004/10/05 02:05:11  tino
 * added (prototypes)
 */

#ifndef tino_INC_io_h
#define tino_INC_io_h

#define TINO_FILE_EXCEPTION
#include "file.h"
#include "exception.h"
#include "alloc.h"
#include "hash.h"

#include <sys/types.h>
#include <sys/socket.h>

/* Never even try to access or predict this.
 * It will be renamed/renumbered unconditionally!
 */
#define N	123123
#define	I	tino_io_##N

/* Note that 0 always is stdin, 1 always is stdout and 2 always is stderr
 * Even if you close it ;)
 */
static struct tino_io_glob
  {
    struct tino_io_imp	imp;
    int		fd;	/* -1: unused, -2:freed due to fd shortage	*/
    int		type;	/* internal type, 0 if not set	*/
  } *I;
static int	tino_io_fds;

static void
tino_io_init(int fds, void *thread_key)
{
  /*I	= ptr ? ;*/
  000;
}

static int
tino_io_new(const char *type)
{
  int	fd;
  int	i;

  /* Try to dup something
   */
  fd	= dup(2);
  if (fd<0)
    fd	= socket(PF_LOCAL, SOCK_DGRAM, 0);
  if (fd<0 || fd>0x10000000)
    tino_throw(TINO_EX_IO, "new");

  /* expand the working array
   */
  if (fd>=tino_io_fds)
    {
      int	i;

      i	= fd+256;
      I	= tino_realloc(I, i*sizeof *I);
      while (tino_io_fds<i)
	I[tino_io_fds++].fd	= -1;
    }

  /* The fd is already taken internally,
   * hunt for a free slot ..
   */
  if (I[fd].fd==-1)
    {
      000;
    }

  for (i=3; i<tino_io_fds; i++)
    {
      000;
    }
  000;
}

/* This can return -1 for error/not allocated,
 * or -2 if fd was closed, perhaps due to fd shortage.
 *
 * Usually tino_io_fd(x)==x.
 * Sometimes it might not:
 * When you open special IO things, or when your system runs out of file handles.
 * tino_io is thought to be able to handle this case gracefully,
 * sometimes, in the future.
 */
static int
tino_io_fd(int io)
{
  if (io<0 || io>=tino_io_fds)
    return -1;
  return I[io].fd;
}

/* Tell if EOF or ERROR condition:
 * Returns:
 * 1 if EOF,
 * 0 if no EOF and OK,
 * -1 if error state.
 */
static int
tino_io_eof(int io)
{
  000;
  return 1;
}

/* Close IO:
 * Returns:
 * 0 if OK,
 * -1 if error state.
 */
static int
tino_io_close(int io)
{
  000;
  return -1;
}

/* Do some processing.
 * Return if something is processed.
 * Returns number of signals received (EINTR) while everything is processed.
 * This function blocks maximum timeout seconds.
 * With timeout=0 it does not block at all.
 * With timeout=-1 it does block forever.
 */
static int
tino_io_process(int timeout)
{
  sleep(timeout);
  000;
  return 0;
}

static int
tino_io_open(const char *s, const char *mode, ...)
{
  000;
  return -1;
}

static int
tino_io_set(int io, int token, const void *p, size_t len)
{
  000;
  return -1;
}

static int
tino_io_set_ul(int io, int token, unsigned long ul)
{
  return tino_io_set(io, token, &ul, sizeof ul);
}

static int
tino_io_token(const char *s)
{
  return tino_token(s)+N;
}

static void
tino_io_copy(int in, int out)
{
  000;
}

#undef I
#undef N

#endif
