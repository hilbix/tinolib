/* $Header$
 *
 * $Log$
 * Revision 1.3  2004-04-13 10:51:54  tino
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

#include "alloc.h"
#include "codec.h"

typedef struct tino_buf
  {
    size_t	fill, max;
    char	*data;
  } TINO_BUF;

#define TINO_BUF_ADD_C(buf,c) do { if ((buf)->fill<(buf)->max) tino_buf_extend(buf, BUFSIZ); (buf)->data[(buf)->fill++]=(c); } while (0)

static void
tino_buf_init(TINO_BUF *buf)
{
  buf->fill	= 0;
  buf->max	= 0;
  buf->data	= 0;
}

static void
tino_buf_extend(TINO_BUF *buf, size_t len)
{
  xDP(("tino_buf_extend(%p,%ld) p=%p l=%ld m=%ld", buf, (long)len,
      buf->data, (long)buf->fill, (long)buf->max));
  buf->max	+= len;
  buf->data	= tino_realloc(buf->data, buf->max);
  xDP(("tino_buf_extend() %p", buf->data));
}

static void
tino_buf_reset(TINO_BUF *buf)
{
  buf->fill	= 0;
}

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
      out	-= max;
      if (out<0)
	{
	  buf->fill	+= out;
	  return buf->data;
	}
      /* There is a bug in older libraries.
       * vsnprintf does not return the size needed,
       * instead it returns max.
       * We cannot distinguish between this case and the case,
       * that just the space for the \0 is missing.
       * Therefor we define to always extend buffer by BUFSIZ at minimum.
       */
      if (++out<BUFSIZ)
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
tino_buf_add(TINO_BUF *buf, const char *s)
{
  tino_buf_add_n(buf, s, strlen(s));
}

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
  return buf->data;
}

static const char *
tino_buf_get(TINO_BUF *buf)
{
  if (!buf)
    return 0;
  return buf->data;
}

static size_t
tino_buf_len(TINO_BUF *buf)
{
  if (!buf)
    return 0;
  return buf->fill;
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
