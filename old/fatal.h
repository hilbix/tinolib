/* $Header$
 *
 * $Log$
 * Revision 1.5  2004-03-28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.4  2004/03/26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.3  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.2  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

#ifndef tino_INC_fatal_h
#define tino_INC_fatal_h

#include "ex.h"

#define	tino_FATAL(X)	do { if (X) { tino_fatal_gen(#X, __FILE__, __LINE__); } } while(0)

static void
tino_fatal(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_verror("fatal error", s, list, 0);
  exit(-2);
  abort();
  for(;;);
}

static void
tino_fatal_gen(const char *s, const char *file, int line)
{
  tino_fatal("%s:%d: %s", file, line, s);
}

#endif
