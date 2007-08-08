/* $Header$
 *
 * DATA processing handles
 *
 * With this wrapper you can either wrap TINO_BUF or files.  In future
 * more handlers will be added.  You can use it statically, but then
 * you must free the buffer yourself.
 *
 * Example:
 *
 * Circular: TINO_DATA *d1=tino_data_buf(NULL);
 * Bidirect: TINO_DATA *d2=tino_data_buf2(NULL, d1);
 *
 * TINO_DATA *d=tino_data_file(NULL, tino_file_open_create(name, O_APPEND, 0664));
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.7  2007-08-08 11:26:12  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.6  2007/04/14 22:58:18  tino
 * typo
 *
 * Revision 1.5  2007/04/08 10:26:02  tino
 * tino_data_printf and tino_data_write_escape
 *
 * Revision 1.4  2007/01/18 20:07:04  tino
 * tino_va_list and TINO_VA_LIST changes
 *
 * Revision 1.3  2006/11/10 01:02:33  tino
 * Updated to changes added recently
 *
 * Revision 1.2  2006/08/24 01:51:28  tino
 * improved usability for tino_data_*
 *
 * Revision 1.1  2006/08/14 04:21:13  tino
 * Changes for the new added curl.h and data.h
 */

#ifndef tino_INC_data_h
#define tino_INC_data_h

#include "file.h"
#include "buf_printf.h"

#define	cDP	TINO_DP_data

typedef struct tino_data	TINO_DATA;

struct tino_data
  {
    TINO_BUF			buf;
    struct tino_data_handler	*handler;
    void			*user;
    void			(*err)(TINO_DATA *, TINO_VA_LIST list);
    void			(*intr)(TINO_DATA *);
    int				allocated;
  };

struct tino_data_handler
  {
    const char		*type;
    int			(*read)(TINO_DATA *, void *, size_t);
    int			(*write)(TINO_DATA *, const void *, size_t);
    tino_file_size_t	(*pos)(TINO_DATA *);
    tino_file_size_t	(*size)(TINO_DATA *);
    tino_file_size_t	(*seek)(TINO_DATA *, tino_file_size_t pos);
    tino_file_size_t	(*seek_end)(TINO_DATA *);
    void		(*free)(TINO_DATA *);
    void		(*init)(TINO_DATA *, void *);
  };

/** Set error handling function
 *
 * This function shall not return if you want to abort.  If it
 * returns, the error condition will be ignored.
 */
static void
tino_data_errfn(TINO_DATA *d, void (*err)(TINO_DATA *, TINO_VA_LIST list))
{
  d->err	= err;
}

/** Set interrupt handler
 *
 * This is called if EINTR is encountered.
 *
 * Note: Some signals can be received without returning EINTR
 */
static void
tino_data_intrfn(TINO_DATA *d, void (*intr)(TINO_DATA *))
{
  d->intr	= intr;
}

static void
tino_data_error(TINO_DATA *d, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  if (d && d->err)
    d->err(d, &list);
  else
    tino_vpexit((d->handler ? d->handler->type : "tino_data"), &list);
  tino_va_end(list);
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
  if (!d)
    d	= tino_data_new(user);
  d->handler	= handler;
  if (handler && handler->init)
    handler->init(d, user);
  else
    d->user	= user;
  return d;
}

/** read data into buffer, never returns <0
 *
 * It may read less than requested!
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
  while ((n=d->handler->read(d, ptr, max))<0 && errno==EINTR)
    d->intr(d);
  if (n<0)
    {
      tino_data_error(d, "general read error %d", n);
      return 0;
    }
  return n;
}

/** read data into buffer, never returns <0
 *
 * It reads the exact read count
 */
static void
tino_data_read_all(TINO_DATA *d, void *_ptr, size_t len)
{
  int	pos;
  char	*ptr	= _ptr;

  for (pos=0; pos<len; )
    pos	+= tino_data_read(d, ptr+pos, len-pos);
}

static tino_file_size_t
tino_data_pos(TINO_DATA *d)
{
  if (!d || !d->handler || !d->handler->pos)
    {
      tino_data_error(d, "pos not defined");
      return 0;
    }
  return d->handler->pos(d);
}

static tino_file_size_t
tino_data_size(TINO_DATA *d)
{
  if (!d || !d->handler || !d->handler->size)
    {
      tino_data_error(d, "size not defined");
      return 0;
    }
  return d->handler->size(d);
}

static void
tino_data_seek(TINO_DATA *d, tino_file_size_t pos)
{
  if (!d || !d->handler || !d->handler->seek)
    {
      tino_data_error(d, "seek not defined");
      return;
    }
  if (d->handler->seek(d, pos)!=pos)
    tino_data_error(d, "general seek error");
}

static tino_file_size_t
tino_data_size_generic(TINO_DATA *d)
{
  tino_file_size_t	pos, len;

  if (!d || !d->handler || !d->handler->seek_end)
    {
      tino_data_error(d, "cannot seek to get size");
      return 0;
    }
  pos	= tino_data_pos(d);
  len	= d->handler->seek_end(d);
  tino_data_seek(d, pos);
  return len;
}

/** write out data
 *
 * This writes all data all times
 */
static void
tino_data_write(TINO_DATA *d, const void *ptr, size_t len)
{
  int	pos, put;

  if (!d || !d->handler || !d->handler->write)
    {
      tino_data_error(d, "write not defined");
      return;
    }
  put	= -1;
  for (pos=0; pos<len; )
    {
      errno	= 0;
      if ((put=d->handler->write(d, ptr+pos, len-pos))>0)
	pos	+= put;
      else if (!put || errno!=EINTR)
	break;
      else
	d->intr(d);
    }
  if (pos!=len)
    tino_data_error(d, "general write error %d", put);
}

/** Print out a string (varargs version).
 */
static void
tino_data_vsprintf(TINO_DATA *d, TINO_VA_LIST list)
{
  TINO_BUF	buf;

  tino_buf_init(&buf);
  tino_buf_add_vsprintf(&buf, list);
  tino_data_write(d, tino_buf_get(&buf), tino_buf_get_len(&buf));
  tino_buf_free(&buf);
}

/** Print out a string.
 */
static void
tino_data_printf(TINO_DATA *d, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_data_vsprintf(d, &list);
  tino_va_end(list);
}

/** Convenience routine (speedup)
 */
static void
tino_data_puts(TINO_DATA *d, const char *s)
{
  tino_data_write(d, s, strlen(s));
}

/** Escape a string
 *
 * 'escape' will be doubled, "escaped" are characters which are
 * escaped by 'escape'
 */
static void
tino_data_write_escape(TINO_DATA *d, const char *s, char escape, const char *escaped)
{
  int	loop;

  for (loop=0; *s; loop=1)
    {
      const char	*ptr;

      ptr	= strchr(s+loop, escape);
      if (escaped)
	{
	  const char	*tmp;

	  tmp	= strpbrk(s, escaped);
	  if (!ptr || (tmp && tmp<ptr))
	    ptr	= tmp;
	}
      if (!ptr)
	ptr	= s+strlen(s);
      tino_data_write(d, s, ptr-s);
      s	= ptr;
    }
}


/**********************************************************************/

static int
tino_data_buf_read(TINO_DATA *d, void *ptr, size_t max)
{
  return tino_buf_fetch(&d->buf, ptr, max);
}

/* If we are connected to another buf write into the other buffer.
 */
static int
tino_data_buf_write(TINO_DATA *d, const void *ptr, size_t max)
{
  tino_buf_add_n((d->user ? &((TINO_DATA *)d->user)->buf : &d->buf), ptr, max);
  return max;
}

static void
tino_data_buf_free(TINO_DATA *d)
{
  if (!d->user)
    return;
  ((TINO_DATA *)d->user)->user	= 0;
  d->user			= 0;
}

struct tino_data_handler tino_data_buf_handler	=
  {
    "tino_data memory buffer",
    tino_data_buf_read,
    tino_data_buf_write,
    0,
    0,
    0,
    0,
    tino_data_buf_free,
    0
  };

#define	TINO_DATA_BUF	(&tino_data_buf_handler)

/* A buffer connected to another buffer
 */
static TINO_DATA *
tino_data_buf2(TINO_DATA *d, TINO_DATA *second)
{
  tino_FATAL(second && (second->handler!=TINO_DATA_BUF || second->user));
  d	= tino_data_handler(d, TINO_DATA_BUF, second);
  if (second)
    second->user	= d;
  return d;
}

/* Circular buffer
 */
static TINO_DATA *
tino_data_buf(TINO_DATA *d)
{
  return tino_data_buf2(d, NULL);
}


/**********************************************************************/

static int
tino_data_file_read(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  n	= tino_file_read_intr((int)d->user, ptr, max);
  if (n<0 && errno==EINTR)
    {
      tino_data_error(d, "file read error fd %d", (int)d->user);
      n	= 0;
    }
  return n;
}

static int
tino_data_file_write(TINO_DATA *d, const void *ptr, size_t max)
{
  int	n;

  n	= tino_file_write_intr((int)d->user, ptr, max);
  if (n<0 && errno==EINTR)
    {
      tino_data_error(d, "file write error fd %d", (int)d->user);
      n	= 0;
    }
  return n;
}

static tino_file_size_t
tino_data_file_pos(TINO_DATA *d)
{
  return tino_file_lseek((int)d->user, (tino_file_size_t)0, SEEK_CUR);
}

static tino_file_size_t
tino_data_file_seek(TINO_DATA *d, tino_file_size_t pos)
{
  return tino_file_lseek((int)d->user, pos, SEEK_SET);
}

static tino_file_size_t
tino_data_file_seek_end(TINO_DATA *d)
{
  return tino_file_lseek((int)d->user, (tino_file_size_t)0, SEEK_END);
}

static void
tino_data_file_close(TINO_DATA *d)
{
  if (close((int)d->user))
    tino_data_error(d, "file close error fd %d", (int)d->user);
}

struct tino_data_handler tino_data_file_handler	=
  {
    "tino_data file handle",
    tino_data_file_read,
    tino_data_file_write,
    tino_data_file_pos,
    tino_data_size_generic,
    tino_data_file_seek,
    tino_data_file_seek_end,
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
  return tino_data_handler(d, TINO_DATA_FILE, (void *)fd);
}


/**********************************************************************/

static int
tino_data_stream_read(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  n	= fread(ptr, 1, max, (FILE *)d->user);
  if (n==0 && ferror((FILE *)d->user))
    tino_data_error(d, "filestream read error");
  return n;
}

static int
tino_data_stream_write(TINO_DATA *d, const void *ptr, size_t max)
{
  int	n;

  n	= fwrite(ptr, 1, max, (FILE *)d->user);
  if (n==0 && ferror((FILE *)d->user))
    tino_data_error(d, "filestream write error");
  return n;
}

static tino_file_size_t
tino_data_stream_pos(TINO_DATA *d)
{
  return tino_file_ftell((FILE *)d->user);
}

static tino_file_size_t
tino_data_stream_seek(TINO_DATA *d, tino_file_size_t pos)
{
  return tino_file_fseek((FILE *)d->user, pos, SEEK_SET) ? pos+1 : pos;
}

static tino_file_size_t
tino_data_stream_seek_end(TINO_DATA *d)
{
  return tino_file_fseek((FILE *)d->user, (tino_file_size_t)0, SEEK_END);
}

static void
tino_data_stream_close(TINO_DATA *d)
{
  if (fclose((FILE *)d->user))
    tino_data_error(d, "stream close error");
}

struct tino_data_handler tino_data_stream_handler	=
  {
    "tino_data file stream pointer",
    tino_data_stream_read,
    tino_data_stream_write,
    tino_data_stream_pos,
    tino_data_size_generic,
    tino_data_stream_seek,
    tino_data_stream_seek_end,
    tino_data_stream_close,
    0
  };

#define	TINO_DATA_STREAM	(&tino_data_stream_handler)

static TINO_DATA *
tino_data_stream(TINO_DATA *d, FILE *fd)
{
  if (!fd)
    {
      tino_data_error(d, "filestream open error");
      return d;
    }
  return tino_data_handler(d, TINO_DATA_STREAM, (void *)fd);
}


/**********************************************************************/

#undef cDP
#endif
