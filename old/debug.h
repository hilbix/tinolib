/* $Header$
 *
 * If you include this, you know what you are doing.
 *
 * $Log$
 * Revision 1.1  2004-03-28 01:24:14  tino
 * some more stuff
 *
 */

#ifndef tino_INC_debug_h
#define tino_INC_debug_h

#include <stdio.h>
#include <stdarg.h>

#define DP(X)		do { tino_debugprintf X; } while (0)
#define xDP(X)		do { ; } while (0)

static void
tino_debugprintf(const char *s, ...)
{
  va_list	list;

  fprintf(stderr, "[");
  va_start(list, s);
  vfprintf(stderr, s, list);
  va_end(list);
  fprintf(stderr, "]\n");
}

#endif
