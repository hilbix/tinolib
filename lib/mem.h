/* MiniLib
 *
 * Memory management
 */

#include "def.h"

#include <stdlib.h>

#include "err.h"
#include "out.h"

static void *
re_alloc(void *buf, size_t len)
{
  buf	= realloc(buf, len);
  if (!buf)
    OOPS("out of memory allocating ", OUT(len), " bytes");
  return buf;
}

static void *
alloc(size_t len)
{
  return re_alloc(NULL, len);
}

static void *
alloc0(size_t len)
{
  void *ptr;

  ptr	= alloc(len);
  memset(ptr, 0, len);
  return ptr;
}

