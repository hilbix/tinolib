/* $Header$
 *
 * Async IO routines
 *
 * $Log$
 * Revision 1.1  2004-08-18 16:00:45  Administrator
 * AIO not available under CygWin
 *
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
