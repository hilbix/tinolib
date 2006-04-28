/* $Header$
 *
 * Data buffers (allocated, growing)
 *
 * Usage:
 *	TINO_BUF buf;
 *	tino_buf_init(&buf);
 *	...;
 *	tino_buf_free(&buf);
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
 * Revision 1.15  2006-04-28 11:45:35  tino
 * va_copy now via sysfix.h (still incomplete!) and
 * buf_add_sprintf() etc. now in separate include
 *
 * Revision 1.14  2006/04/11 21:42:12  tino
 * *** empty log message ***
 *
 * Revision 1.13  2006/01/07 18:03:41  tino
 * tino_buf_write_away changed to better fulfill needs
 *
 * Revision 1.12  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.11  2005/12/03 13:41:41  tino
 * extended to (hopefully) working writes, reads and more helpers
 *
 * Revision 1.10  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.9  2004/08/17 23:06:58  Administrator
 * Minor (not yet used parts) bugs removed and added functions
 *
 * Revision 1.8  2004/07/17 22:25:34  tino
 * bug removed
 *
 * Revision 1.7  2004/06/13 03:48:04  tino
 * little modifications
 *
 * Revision 1.6  2004/06/12 06:30:11  tino
 * xml2gff bugfix (deleted structs), new tinolib version (untested)
 *
 * Revision 1.5  2004/05/19 05:00:04  tino
 * idea added
 *
 * Revision 1.4  2004/05/01 01:42:28  tino
 * offset added
 *
 * Revision 1.3  2004/04/13 10:51:54  tino
 * Starts to work like it seems
 *
 * Revision 1.2  2004/04/08 21:38:36  tino
 * Further improvements for SQL writing.  Some BUGs removed, too.
 *
 * Revision 1.1  2004/04/07 02:22:48  tino
 * Prototype for storing data in gff_lib done (untested)
 */

#ifndef tino_INC_buf_h
#define tino_INC_buf_h

#include <unistd.h>

#include "alloc.h"
#include "codec.h"
#include "fatal.h"

#define	cDP	TINO_DP_buf

typedef struct tino_buf
  {
    size_t	fill;	/* Usual acutal fill position	*/
    size_t	max;	/* Maximum alocated data size	*/
    size_t	off;	/* Offset from the begining which is free	*/
    char	*data;
  } TINO_BUF;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* internal functions */

#define TINO_BUF_ADD_C(buf,c) do { if ((buf)->fill>=(buf)->max) tino_buf_extend(buf, BUFSIZ); (buf)->data[(buf)->fill++]=(c); } while (0)

static void
tino_buf_extend(TINO_BUF *buf, size_t len)
{
  cDP(("tino_buf_extend(%p,%ld) p=%p l=%ld m=%ld p=%ld", buf, (long)len,
      buf->data, (long)buf->fill, (long)buf->max, (long)buf->off));
  if (!len)
    return;
  if (buf->off>=len)
    {
      tino_FATAL(buf->fill<buf->off);
      if ((buf->fill-=buf->off)!=0)
	memmove(buf->data, buf->data+buf->off, buf->fill);
      buf->off	= 0;
    }
  else
    {
      buf->max	+= len;
      buf->data	=  tino_realloc(buf->data, buf->max);
    }
  cDP(("tino_buf_extend() %p", buf->data));
}

#define TINO_BUF_PREPEND_C(buf,c) do { if ((buf)->off<=0) tino_buf_prepend(buf, BUFSIZ); (buf)->data[--(buf)->off]=(c); } while (0)

static void
tino_buf_prepend(TINO_BUF *buf, size_t len)
{
  cDP(("tino_buf_prepend(%p,%ld) p=%p l=%ld m=%ld p=%ld", buf, (long)len,
      buf->data, (long)buf->fill, (long)buf->max, (long)buf->off));

  if (buf->off>=len)
    return;

  tino_FATAL(buf->fill<buf->off);
  if (buf->fill-buf->off+len>buf->max)
    {
      buf->max	+= len;	/* this leaves buf->off free room at the end	*/
      buf->data	=  tino_realloc(buf->data, buf->max);
    }
  if ((buf->fill-=buf->off)!=0)
    memmove(buf->data+len, buf->data+buf->off, buf->fill);
  buf->fill	+= len;
  buf->off	= len;
  cDP(("tino_buf_prepend() %p", buf->data));
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* misc functions */

static void
tino_buf_reset(TINO_BUF *buf)
{
  cDP(("tino_buf_reset(%p)", buf));
  buf->fill	= 0;
  buf->off	= 0;
}

static void
tino_buf_init(TINO_BUF *buf)
{
  cDP(("tino_buf_init(%p)", buf));
  tino_buf_reset(buf);
  buf->max	= 0;
  buf->data	= 0;
}

static void
tino_buf_free(TINO_BUF *buf)
{
  cDP(("tino_buf_free(%p)", buf));
  if (buf && buf->data)
    free(buf->data);
  tino_buf_init(buf);
}

static void
tino_buf_swap(TINO_BUF *a, TINO_BUF *b)
{
  TINO_BUF	x;

  cDP(("tino_buf_swap(%p,%p)", a, b));
  x	= *a;
  *a	= *b;
  *b	= x;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* prepend functions (add at the beginning) */

static void
tino_buf_prepend_c(TINO_BUF *buf, char c)
{
  cDP(("tino_buf_prepend_c(%p,%d)", buf, c));
  TINO_BUF_PREPEND_C(buf, c);
}

static __inline__ char *
tino_buf_prepend_ptr(TINO_BUF *buf, size_t len)
{
  cDP(("tino_buf_prepend_ptr(%p,%ld)", buf, (long)len));
  if (buf->off<len)
    tino_buf_prepend(buf, len);
  return buf->data+buf->off-len;
}

static void
tino_buf_prepend_n(TINO_BUF *buf, const void *ptr, size_t len)
{
  cDP(("tino_buf_prepend_n(%p,%p,%ld)", buf, ptr, (long)len));
  memcpy(tino_buf_prepend_ptr(buf, len), ptr, len);
  buf->off	-= len;
}

static void
tino_buf_prepend_s(TINO_BUF *buf, const char *s)
{
  cDP(("tino_buf_prepend_s(%p,'%s')", buf, s));
  tino_buf_prepend_n(buf, s, strlen(s));
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* add functions */

static void
tino_buf_add_c(TINO_BUF *buf, char c)
{
  cDP(("tino_buf_add_c(%p,%d)", buf, c));
  TINO_BUF_ADD_C(buf, c);
}

static __inline__ char *
tino_buf_add_ptr(TINO_BUF *buf, size_t len)
{
  cDP(("tino_buf_add_ptr(%p,%ld)", buf, (long)len));
  if (buf->fill+len>=buf->max)
    tino_buf_extend(buf, buf->fill+len-buf->max+1);
  return buf->data+buf->fill;
}

static void
tino_buf_add_n(TINO_BUF *buf, const void *ptr, size_t len)
{
  cDP(("tino_buf_add_n(%p,%p,%ld)", buf, ptr, (long)len));
  memcpy(tino_buf_add_ptr(buf, len), ptr, len);
  buf->fill	+= len;
}

static void
tino_buf_add_s(TINO_BUF *buf, const char *s)
{
  cDP(("tino_buf_add_s(%p,'%s')", buf, s));
  tino_buf_add_n(buf, s, strlen(s));
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* get functions */

static const char *
tino_buf_get_s(TINO_BUF *buf)
{
  cDP(("tino_buf_get_s(%p)", buf));
  if (!buf)
    return 0;
  if (!buf->fill)
    return "";
  if (buf->fill>=buf->max)
    tino_buf_extend(buf, 1);
  buf->data[buf->fill]	= 0;
  cDP(("tino_buf_get_s() '%s'", buf->data+buf->off));
  return buf->data+buf->off;
}

static const char *
tino_buf_get(TINO_BUF *buf)
{
  cDP(("tino_buf_get(%p)", buf));
  if (!buf)
    return 0;
  cDP(("tino_buf_get() %p", buf->data+buf->off));
  return buf->data+buf->off;
}

static int
tino_buf_get_c(TINO_BUF *buf)
{
  cDP(("tino_buf_get_c(%p)", buf));
  if (!buf || buf->off>=buf->fill)
    return EOF;
  cDP(("tino_buf_get_c() %d", buf->data[buf->off]));
  return (unsigned char)buf->data[buf->off++];
}

static size_t
tino_buf_get_len(TINO_BUF *buf)
{
  if (!buf)
    {
      cDP(("tino_buf_get_len(%p) 0", buf));
      return 0;
    }
  cDP(("tino_buf_get_len(%p) %ld", buf, (long)(buf->fill-buf->off)));
  return buf->fill-buf->off;
}

/* How much history is there in the buffer?
 */
static size_t
tino_buf_get_history(TINO_BUF *buf)
{
  cDP(("tino_buf_get_history(%p)", buf));
  if (!buf)
    return 0;
  return buf->off;
}

/* You can get negative values as long as it's still in the buffer
 * If not, you have a FATAL.
 * Returns the number of bytes left in the buffer.
 */
static int
tino_buf_advance_n(TINO_BUF *buf, int max)
{
  cDP(("tino_buf_advance_n(%p,%d)", buf, max));
  if (!buf)
    return 0;
  max	+= buf->off;
  tino_FATAL(max<0);
  if (max>buf->fill)
    buf->off	= buf->fill;
  else
    buf->off	= max;
  return buf->fill-buf->off;
}

static int
tino_buf_advance(TINO_BUF *buf, int n)
{
  tino_FATAL(n<0);
  return tino_buf_advance_n(buf, n);
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* IO functions */

static int
tino_buf_read(TINO_BUF *buf, int fd, int max)
{
  char	*ptr;
  int	got;

  cDP(("tino_buf_read(%p,%d,%d)", buf, fd, max));
  if (max<0)
    max	= BUFSIZ*10;
  ptr	= tino_buf_add_ptr(buf, max);
  got	= read(fd, ptr, max);
  if (got>0)
    buf->fill	+= got;
  cDP(("tino_buf_read() %d", got));
  return got;
}

/* Convenience routine:
 *
 * Read everything until EOF and ignores EINTR.
 * max is the upper limit of data read, may be -1 for "unlimited"
 *
 * Returns:
 * -1	on error
 * 0	on EOF (no input)
 * len	nuber of bytes read
 */
static int
tino_buf_read_all(TINO_BUF *buf, int fd, int max)
{
  int	block, total;

  cDP(("tino_buf_read_all(%p,%d,%d)", buf, fd, max));
  /* For efficiency increase the blocksize slowly after successful
   * reads.  Such less memory reallocations and less kernel calls are
   * done.  As soon as any short read is seen, the blocksize is reset
   * to BUFSIZ again.
   */
  total	= 0;
  block	= 0;
  for (;;)
    {
      int	len, got;

      if (block<BUFSIZ*256)	/* 4 KB to 1 MB	*/
	block	+= BUFSIZ;
      len	= block;
      if (max>=0 && len>max-total)
	len	= max-total;
      if (!len)
	break;

      /* do the read
       */
      got	= tino_buf_read(buf, fd, len);

      if (got<=0)
	{
	  if (!got)
	    break;
	  if (errno!=EAGAIN && errno!=EINTR)
	    return -1;
	}
      else
	total	+= got;

      /* reset blocksize on short read
       */
      if (got<block)
	block	= 0;
    }
  cDP(("tino_buf_read_all() %d", total));
  return total;
}

/**********************************************************************
 * There are too many write routines now.
 * Expect some to vanish in future.
 * The problem was, I needed so many different "easy" variants,
 * that it became difficult to find the right one now.
 *
 * tino_buf_write_away() will stay (and perhaps be renamed
 * tino_buf_write() sometimes)
 **********************************************************************/

/* Convenience routine:
 *
 * Write complete buffer to FD, ignore EINTR, but do not advance
 * buffer pointer!  So use as follows:
 *
 * if (tino_buf_write_all(buf, fd))
 *   error(...);
 * tino_buf_reset(buf);
 *
 * Returns:
 * -1		on error
 * 0		if everything written (or empty buffer)
 * len+1	number of short bytes (usually error, too)
 */
static int
tino_buf_write_all_1(TINO_BUF *buf, int fd)
{
  int	len, n, put;

  cDP(("tino_buf_write_all(%p,%d)", buf, fd));
  len	= tino_buf_get_len(buf);
  for (n=0; n<len; )
    {
      if ((put=write(fd, tino_buf_get(buf)+n, len-n))>0)
	n	+= put;
      else
	{
	  if (!put)
	    {
	      cDP(("tino_buf_write_all() %d", n));
	      return n+1;
	    }
	  if (errno!=EINTR && errno!=EAGAIN)
	    {
	      cDP(("tino_buf_write_all() -1"));
	      return -1;
	    }
	}
    }
  cDP(("tino_buf_write_all() 0"));
  return 0;
}

/* Convenience routine:
 *
 * Write complete buffer to FD, ignore EINTR and advance buffer.
 * set max=-1 to write all available data.
 *
 * if (tino_buf_write_away_all(buf, fd, -1))
 *   error(...);
 *
 * Returns:
 * -1	on error
 * 0	if everything written (or empty buffer)
 * 1	short write
 */
static int
tino_buf_write_away_all(TINO_BUF *buf, int fd, int max)
{
  cDP(("tino_buf_write_all_away(%p,%d,%d)", buf, fd, max));
  for (;;)
    {
      int	len, put;

      len	= tino_buf_get_len(buf);
      if (max>=0 && len>max)
	len	= max;
      if (!len)
	{
	  cDP(("tino_buf_write_all_away() 0"));
	  return 0;
	}
      cDP(("tino_buf_write_all_away() write %d", len));
      if ((put=write(fd, tino_buf_get(buf), len))>0)
	{
	  max	-= put;
	  tino_buf_advance(buf, put);
	}
      else if (!put)
	{
	  cDP(("tino_buf_write_all_away() 1"));
	  return 1;
	}
      else if (errno!=EINTR && errno!=EAGAIN)
	{
	  cDP(("tino_buf_write_all_away() -1"));
	  return -1;
	}
    }
}

#if 1
/* -1 on error, returns 0 on EOF, 1 if ok
 * If ok, max is set to the bytes written, this is <0 on EINTR.
 *
 * If you set max <0 or to NULL then all available data is written (or
 * as much as is accepted by the stream).
 */
static int
tino_buf_write_eof_0(TINO_BUF *buf, int fd, int *max)
{
  int	put;

  cDP(("tino_buf_write_eof_0(%p,%d,%p[%d])", buf, fd, max, (max ? *max : 0)));
  put	= tino_buf_get_len(buf);
  if (max && put>*max && *max>=0)
    put	= *max;
  if (put)
    {
      put	= write(fd, tino_buf_get(buf), put);
      if (!put)
	{
	  cDP(("tino_buf_write_eof_0() 0"));
	  return 0;
	}
      if (put<0 && (errno!=EAGAIN && errno!=EINTR))
	{
	  cDP(("tino_buf_write_eof_0() -1"));
	  return -1;
	}
    }
  if (max)
    *max	= put;
  cDP(("tino_buf_write_eof_0() 1"));
  return 1;
}
#else
/* returns 1 on EOF, -1 on error, 0 if ok
 * If ok, max is set to the bytes written, this is <0 on EINTR.
 *
 * If you set max <0 or to NULL then all available data is written.
 */
static int
tino_buf_write_eof(TINO_BUF *buf, int fd, int *max)
{
  int	put;

  put	= tino_buf_get_len(buf);
  if (max && put>*max && *max>=0)
    put	= *max;
  if (put)
    {
      put	= write(fd, tino_buf_get(buf), put);
      if (!put)
	return 1;
      if (put<0 && (errno!=EAGAIN && errno!=EINTR))
	return -1;
    }
  if (max)
    *max	= put;
  return 0;
}
#endif

/* Write something.  -1=all
 * Returns:
 * -1	error (EAGAIN if buffer is empty)
 * 0	EOF
 * >0	number bytes written
 */
static int
tino_buf_write_away(TINO_BUF *buf, int fd, int max)
{
  int	put;

  cDP(("tino_buf_write_away(%p,%d,%d)", buf, fd, max));
  put	= tino_buf_get_len(buf);
  if (max>=0 && put>max)
    put	= max;
  if (!put)
    {
      cDP(("tino_buf_write_away() -1 (EAGAIN)", put));
      errno	= EAGAIN;
      return -1;
    }
  put	= write(fd, tino_buf_get(buf), put);
  if (put>0)
    tino_buf_advance(buf, max);
  cDP(("tino_buf_write_away() %d", put));
  return put;
}

static void
tino_buf_write_out(TINO_BUF *buf, int fd)
{
  int	max;

  max	= -1;
  tino_buf_write_eof(buf, fd, &max);
}

#if 1
/* Convenienc routine:
 *
 * Write buffered data to FD.
 * returns:
 * -3 on EOF (like: The other side closed the pipe)
 * -2 on error
 * -1 on nothing to write
 * else: number of bytes written
 *
 * The number of bytes written may be short (less than max),
 * in which case it's likely a signal was cought.
 *
 * Hint:
 *
 * while ((put=tino_buf_write_ok(buf, fd, -1))>=0)
 *   {
 *     tino_buf_advace(buf, put);
 *     check_for_signals();
 *   }
 * if (put==-1)
 *   return;
 * ex("write error");
 */
static int
tino_buf_write_ok(TINO_BUF *buf, int fd, int max)
{
  int	ret;

  cDP(("tino_buf_write_ok(%p,%d,%d)", buf, fd, max));
  ret	= tino_buf_write_eof_0(buf, fd, &max);
  if (ret<=0)
    {
      cDP(("tino_buf_write_ok() %d", (ret<0 ? -2 : -3)));
      return (ret<0 ? -2 : -3);
    }

  /* We now know that MAX is meaningful
   */
  if (max<=0)
    {
      cDP(("tino_buf_write_ok() %d", (max<0 ? 0 : -1)));
      return (max<0 ? 0 : -1);
    }
  cDP(("tino_buf_write_ok() %d", max));
  return max;
}
#else
/* Actually following old implementation was bullshit.
 * If the write is interrupted it may be short.
 * In this case you need to know how much was written!
 */
THIS WILL BE DELETED AS SOON AS I HAVE CROSS-CHECKED EVERYTHING
/* Convenienc routine:
 * returns
 * -1 on error
 * 0 on EOF
 * 1 on EINTR
 * 2 on nothing written (max=0 or nothing to do)
 * 3 on something written
 */
static int
tino_buf_write(TINO_BUF *buf, int fd, int max)
{
  int	ret;

  ret	= tino_buf_write_eof(buf, fd, &max);
  if (ret)
    return (ret<0 ? ret : 0);
  /* We now know that MAX is meaningful
   */
  if (max<=0)
    return (!max ? 2 : 1);
  return 3;
}
#endif

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Specials */

static int
tino_buf_add_hex(TINO_BUF *buf, const char *s)
{
  size_t	len;
  char		*tmp;
  int		i;

  cDP(("tino_buf_add_hex(%p,'%s')", buf, s));
  if (!buf || !s)
    return -1;
  len	= strlen(s);
  tmp	= tino_buf_add_ptr(buf, len);
  i	= tino_dec_hex(tmp, len, s);
  buf->fill	+= i;
  return s[i+i] ? 1 : 0;
}

#if 0
/* Escape the buffer.
 * Each byte found in the buffer which can be found in p
 * is prepended with the prefix, therefor escaped.
 * Note that this copies the data into a second buffer,
 * and then both switch positions.
 */
static void
tino_buf_escape_gen(TINO_BUF *buf, const char *p, size_t l,
		    const char *(*fn)(const void *p, size_t l, void *user),
		    void *user)
{
  000;
}

static void
tino_buf_escape_bin(TINO_BUF *buf, const char *p, size_t l, const char *prefix)
{
  tino_buf_escape_gen(buf, p, l, tino_buf_escape_fn_simple, prefix);
}

static void
tino_buf_escape(TINO_BUF *buf, const char *chars, const char *prefix)
{
  tino_buf_escape_bin(buf, chars, strlen(chars), prefix);
}
#endif

#undef	cDP
#endif
