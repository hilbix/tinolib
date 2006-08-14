/* $Header$
 *
 * DATA processing handles
 *
 * You can use it statically, but then you must free the buffer
 * yourself.
 *
 * If no data handler is set, it will work nearly like TINO_BUF.
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
 * Revision 1.1  2006-08-14 04:21:13  tino
 * Changes for the new added curl.h and data.h
 *
 */

#ifndef tino_INC_data_h
#define tino_INC_data_h

#include "file.h"
#include "buf.h"

#define	cDP	TINO_DP_data

typedef struct tino_data	TINO_DATA;

struct tino_data
  {
    TINO_BUF			buf;
    struct tino_data_handler	*handler;
    void			*user;
    void			(*err)(TINO_DATA *, const char *s, va_list list);
    int				allocated;
  };

struct tino_data_handler
  {
    int		(*read)(TINO_DATA *, void *, size_t);
    void	(*write)(TINO_DATA *, const void *, size_t);
    void	(*free)(TINO_DATA *);
    void	(*init)(TINO_DATA *, void *);
  };

static void
tino_data_errfn(TINO_DATA *d, void (*err)(TINO_DATA *, const char *s, va_list list))
{
  d->err	= err;
}

static void
tino_data_error(TINO_DATA *d, const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  if (d->err)
    d->err(d, s, list);
  else
    tino_vpexit("tino_data", s, list);
  va_end(list);
}

static TINO_DATA *
tino_data_new(void *user)
{
  TINO_DATA	*d;

  d		= tino_alloc0(sizeof *d);
  d->allocated	= 1;
  d->user	= user;
  return d;
}

static void
tino_data_free(TINO_DATA *d)
{
  if (d->handler && d->handler->free)
    d->handler->free(d);
  d->handler	= 0;
  tino_buf_free(&d->buf);
  if (d->allocated)
    free(d);
}

static TINO_DATA *
tino_data_handler(TINO_DATA *d, struct tino_data_handler *handler, void *user)
{
  d->handler	= handler;
  if (handler && handler->init)
    handler->init(d, user);
  else
    d->user	= user;
  return d;
}

/* never returns <0
 */
static int
tino_data_read(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  if (!d || !d->handler || !d->handler->read)
    {
      tino_data_error(d, "read not defined");
      return 0;
    }
  n	= d->handler->read(d, ptr, max);
  if (n<0)
    {
      tino_data_error(d, "general read error %d", n);
      return 0;
    }
  return n;
}

static void
tino_data_write(TINO_DATA *d, const void *ptr, size_t len)
{
  if (!d || !d->handler || !d->handler->write)
    {
      tino_data_error(d, "write not defined");
      return;
    }
  d->handler->write(d, ptr, len);
}


/**********************************************************************/

static int
tino_data_buf_read(TINO_DATA *d, void *ptr, size_t max)
{
  return tino_buf_fetch(&d->buf, ptr, max);
}

static void
tino_data_buf_write(TINO_DATA *d, const void *ptr, size_t max)
{
  tino_buf_add_n(&d->buf, ptr, max);
}

struct tino_data_handler tino_data_buf_handler	=
  {
    tino_data_buf_read,
    tino_data_buf_write,
    0,
    0
  };

#define	TINO_DATA_BUF	(&tino_data_buf_handler)


/**********************************************************************/

static int
tino_data_file_read(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  n	= tino_file_read((int)d->user, ptr, max);
  if (n<0)
    {
      tino_data_error(d, "file read error fd %d", (int)d->user);
      n	= 0;
    }
  return n;
}

static void
tino_data_file_write(TINO_DATA *d, const void *ptr, size_t max)
{
  if (tino_file_write_all((int)d->user, ptr, max)!=max)
    tino_data_error(d, "file write error fd %d", (int)d->user);
}

static void
tino_data_file_close(TINO_DATA *d)
{
  if (close((int)d->user))
    tino_data_error(d, "file close error fd %d", (int)d->user);
}

struct tino_data_handler tino_data_file_handler	=
  {
    tino_data_file_read,
    tino_data_file_write,
    tino_data_file_close,
    0
  };

#define	TINO_DATA_FILE	(&tino_data_file_handler)

static TINO_DATA *
tino_data_file(TINO_DATA *d, int fd)
{
  if (fd<0)
    {
      tino_data_error(d, "file open error");
      return d;
    }
  if (!d)
    d	= tino_data_new(NULL);
  return tino_data_handler(d, TINO_DATA_FILE, (void *)fd);
}


/**********************************************************************/

#undef cDP
#endif
