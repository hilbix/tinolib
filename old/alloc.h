/* $Header$
 *
 * $Log$
 * Revision 1.5  2004-04-20 23:51:38  tino
 * Hashing added (untested!)
 *
 * Revision 1.4  2004/04/13 10:51:54  tino
 * Starts to work like it seems
 *
 * Revision 1.3  2004/03/26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

#ifndef tino_INC_alloc_h
#define tino_INC_alloc_h

#include "ex.h"

static void *
tino_realloc(void *ptr, size_t len)
{
  void	*tmp;

  xDP(("tino_realloc(%p,%ld)", ptr, (long)len));
  tmp	= ptr ? realloc(ptr, len) : malloc(len);
  if (!tmp)
    tino_exit("out of memory");
  xDP(("tino_realloc() %p", tmp));
  return tmp;
}

static void *
tino_realloc0(void *buf, size_t len, size_t increment)
{
  void	*ptr;

  ptr	= tino_realloc(buf, len+increment);
  memset((char *)ptr+len, 0, increment);
  return ptr;
}

static void *
tino_alloc(size_t len)
{
  return tino_realloc(NULL, len);
}

static void *
tino_alloc0(size_t len)
{
  void	*tmp;

  tmp	= tino_alloc(len);
  memset(tmp, 0, len);
  return tmp;
}

static void *
tino_memdup(const void *ptr, size_t len)
{
  void		*tmp;

  tmp	= alloc(len);
  memcpy(tmp, ptr, len);
  return tmp;
}

static char *
tino_strdup(const char *s)
{
  char		*buf;

  buf	= strdup(s);
  if (!buf)
    tino_exit("malloc");
  return buf;
}

static const char *
tino_strset(const char **ptr, const char *s)
{
  if (*ptr)
    free((char *)*ptr);
  return *ptr	= tino_strdup(s);
}

#endif
