/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-04-08 21:38:36  tino
 * Further improvements for SQL writing.  Some BUGs removed, too.
 *
 * Revision 1.1  2004/04/07 02:22:48  tino
 * Prototype for storing data in gff_lib done (untested)
 *
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
  buf->data	= tino_realloc(buf->data, buf->max+=len);
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

#endif
