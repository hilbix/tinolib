/* $Header$
 *
 * Async IO routines
 *
 * NEVER TESTED * (as I did not need it yet)
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
 * Revision 1.2  2005-12-05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.1  2004/08/18 16:00:45  Administrator
 * AIO not available under CygWin
 */

#ifndef tino_INC_file_aio_h
#define tino_INC_file_aio_h

#include <aio.h>

typedef struct aiocb64	tino_file_aio_t;

static int
tino_file_aread(tino_file_aio_t *cbp)
{
  return aio_read64(cbp);
}

static int
tino_file_awrite(tino_file_aio_t *cbp)
{
  return aio_write64(cbp);
}

static int
tino_file_listio(int mode, tino_file_aio_t * const list[], int n,
		  struct sigevent *sig)
{
  return lio_listio64(mode, list, n, sig);
}

static int
tino_file_aerror(const tino_file_aio_t *cbp)
{
  return aio_error64(cbp);
}

static int
tino_file_areturn(tino_file_aio_t *cbp)
{
  return aio_return64(cbp);
}

static int
tino_file_async(int op, tino_file_aio_t *cbp)
{
  return aio_fsync64(op, cbp);
}

static int
tino_file_suspend(const tino_file_aio_t * const list[], int n,
		  const struct timespec *timeout)
{
  return aio_suspend64(list, n, timeout);
}

#endif
