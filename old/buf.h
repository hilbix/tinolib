/* $Header$
 *
 * $Log$
 * Revision 1.8  2004-07-17 22:25:34  tino
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

#define TINO_BUF_ADD_C(buf,c) do { if ((buf)->fill<(buf)->max) tino_buf_extend(buf, BUFSIZ); (buf)->data[(buf)->fill++]=(c); } while (0)

static void
tino_buf_extend(TINO_BUF *buf, size_t len)
{
  xDP(("tino_buf_extend(%p,%ld) p=%p l=%ld m=%ld p=%ld", buf, (long)len,
      buf->data, (long)buf->fill, (long)buf->max, (long)buf->off));
  if (!len)
    return;
  if (buf->off>=len)
    {
      FATAL(buf->fill<buf->off);
      if (buf->fill-=buf->off)
	memmove(buf->data, buf->data+buf->off, buf->fill);
      buf->off	= 0;
    }
  else
    {
      buf->max	+= len;
      buf->data	=  tino_realloc(buf->data, buf->max);
    }
  xDP(("tino_buf_extend() %p", buf->data));
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
  buf->fill	= 0;
  buf->off	= 0;
}

static void
tino_buf_init(TINO_BUF *buf)
{
  tino_buf_reset(buf);
  buf->max	= 0;
  buf->data	= 0;
}

static void
tino_buf_free(TINO_BUF *buf)
{
  if (buf && buf->data)
    free(buf->data);
  tino_buf_init(buf);
}

static void
tino_buf_swap(TINO_BUF *a, TINO_BUF *b)
{
  TINO_BUF	x;

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
/* add functions */

static void
tino_buf_add_c(TINO_BUF *buf, char c)
{
  TINO_BUF_ADD_C(buf, c);
}

static __inline__ char *
tino_buf_add_ptr(TINO_BUF *buf, size_t len)
{
  if (buf->fill+len>=buf->max)
    tino_buf_extend(buf, buf->fill+len-buf->max+1);
  return buf->data+buf->fill;
}

static void
tino_buf_add_n(TINO_BUF *buf, const void *ptr, size_t len)
{
  memcpy(tino_buf_add_ptr(buf, len), ptr, len);
  buf->fill	+= len;
}

static const char *
tino_buf_add_vsprintf(TINO_BUF *buf, const char *s, va_list orig)
{
  tino_buf_add_ptr(buf, strlen(s)+1);
  for (;;)
    {
      va_list	list;
      int	out, max;

      va_copy(list, orig);
      max	= buf->max-buf->fill;
      out	= vsnprintf(buf->data+buf->fill, max, s, list);
      va_end(list);
      FATAL(out<0);
      if (out<max)
	{
	  buf->fill	+= out;
	  return buf->data+buf->off;
	}
      /* There is a bug in older libraries.
       * vsnprintf does not return the size needed,
       * instead it returns max.
       * We cannot distinguish between this case and the case,
       * that just the space for the \0 is missing.
       * Therefor we define to always extend buffer by BUFSIZ at minimum.
       */
      out	= out-max+1;
      if (out<BUFSIZ)
	out	= BUFSIZ;
      tino_buf_extend(buf, out);
    }
}

static const char *
tino_buf_add_sprintf(TINO_BUF *buf, const char *s, ...)
{
  const char	*ret;
  va_list	list;

  va_start(list, s);
  ret	= tino_buf_add_vsprintf(buf, s, list);
  va_end(list);
  return ret;
}

static void
tino_buf_add_s(TINO_BUF *buf, const char *s)
{
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
  if (!buf)
    return 0;
  if (!buf->fill)
    return "";
  if (buf->fill>=buf->max)
    tino_buf_extend(buf, 1);
  buf->data[buf->fill]	= 0;
  return buf->data+buf->off;
}

static const char *
tino_buf_get(TINO_BUF *buf)
{
  if (!buf)
    return 0;
  return buf->data+buf->off;
}

static size_t
tino_buf_get_len(TINO_BUF *buf)
{
  if (!buf)
    return 0;
  return buf->fill-buf->off;
}

/* How much history is there in the buffer?
 */
static size_t
tino_buf_get_history(TINO_BUF *buf)
{
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
  if (!buf)
    return 0;
  max	+= buf->off;
  FATAL(max<0);
  if (max>buf->fill)
    buf->off	= buf->fill;
  else
    buf->off	= max;
  return buf->fill-buf->off;
}

static int
tino_buf_advance(TINO_BUF *buf, int n)
{
  FATAL(n<0);
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

  ptr	= tino_buf_add_ptr(buf, max);
  got	= read(fd, ptr, max);
  if (got>0)
    buf->fill	+= got;
  return got;
}

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

#endif
