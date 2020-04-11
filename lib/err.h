/* MiniLib
 *
 * Common error and debugging processing
 */

#include "def.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void FATAL_(const char *file, int line, const char *fn, const char *what, ...);
#define FATAL(X,...)	do { if (X) FATAL_(__FILE__, __LINE__, __FUNCTION__, #X, ##__VA_ARGS__, NULL); } while (0)

#if 0
static _Noreturn void OOPS_(const char *s, ...);
#define	OOPS(X...)	OOPS_(X, NULL)
#endif

#include "out.h"

#define	xDP(X...)	do {} while (0)
#ifdef DEBUG
#define	DP(X...)	do {} while (0)
#else
#define	DP(X...)	do { errDebugSTDERR(__FILE__, __LINE__, __FUNCTION__, X, NULL); } while (0)
#endif

#ifdef DEBUG
static void
errDebugSTDERR(const char *file, int line, const char *fn, const char *s, ...)
{
  va_list list;

  va_start(list, s);
  OUTwrite(2, "[[", __FILE__, ":", OUT(line), " ", fn, s, OUT(list), "]]", OUTlf);
  va_end(list);
}
#endif

#define	OUTPUT(X...)	OUTPUT_(X, NULL)
static void
OUTPUT_(const char *s, ...)
{
  VA_LIST	list;

  VA_START(list, s);
  OUTput(1, s, OUT(list), OUTlf);
  VA_END(list);
}

#define	OUTERR(X...)	OUTERR_(X, NULL)
static void
OUTERR_(const char *s, ...)
{
  VA_LIST	list;

  VA_START(list, s);
  OUTput(2, s, OUT(list), OUTlf);
  VA_END(list);
}

#define	OUTERRNO(X...)	OUTERRNO_(X, NULL)
static void
OUTERRNO_(const char *s, ...)
{
  int		e = errno;
  VA_LIST	list;

  VA_START(list, s);
  OUTput(2, s, OUT(list), ": ", strerror(e), OUTlf);
  VA_END(list);
}

#define	OOPS(X...)	OOPS_(X, NULL)
static _Noreturn void
OOPS_(const char *s, ...)
{
  static int	wtf;
  VA_LIST	list;

  if (!wtf++)
    {
      VA_START(list, s);
      OUTERRNO("OOPS: ", s, OUT(list));
      VA_END(list);
    }
#if 0
  quick_exit(23);
#endif
  _exit(23); abort(); for(;;) pause();
}

static void
FATAL_(const char *file, int line, const char *fn, const char *what, ...)
{
  VA_LIST list;

  VA_START(list, what);
  OOPS("FATAL error in ", __FILE__, ":", OUT(line), " ", fn, what, OUT(list), OUTlf);
  VA_END(list);
}

