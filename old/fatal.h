/* $Header$
 *
 * $Log$
 * Revision 1.7  2005-01-26 10:51:57  tino
 * Changes for updated exception.h
 *
 * Revision 1.6  2005/01/25 22:14:51  tino
 * exception.h now passes include test (but is not usable).  See ChangeLog
 *
 * Revision 1.5  2004/03/28 00:08:21  tino
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

/* This can be overwritten by exception.h
 */
#ifndef TINO_FATAL
#define	TINO_FATAL(X)	TINO_ERROR_PREFIX(fatal,X)
#define	TINO_VFATAL(X)	TINO_ERROR_PREFIX(vfatal,X)
#endif

/* This is never overwritten, it really terminates.
 */
#define	tino_FATAL(X)	do { if (X) { tino_fatal_gen(#X, __FILE__, __LINE__, __FUNCTION__); } } while(0)

static void
tino_pvfatal(const char *t, const char *s, va_list list)
{
  tino_verror(t, s, list, 0);
  exit(-2);
  abort();
  for(;;);
}

static void
tino_vfatal(const char *s, va_list list)
{
  tino_pvfatal("fatal error", s, list);
  TINO_ABORT(-2);
}

static void
tino_fatal(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_vfatal(s, list);
}

static void
tino_fatal_gen(const char *s, const char *file, int line, const char *function)
{
  tino_fatal("%s:%d:%s: %s", file, line, function, s);
}

#endif
