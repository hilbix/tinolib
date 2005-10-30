/* $Header$
 *
 * If you include this, you know what you are doing:
 *
 * Compile time debugging:
 * DP(("format", args)) prints debug to stderr
 * xDP(("format", args)) prints only if global debugging is on
 *
 * #define TINO_DP_all TINO_DP_ON
 * to enable global debugging
 *
 * #define TINO_DP_main TINO_DP_OFF
 * to disable global debugging (even yours)
 *
 * #define TINO_DP_<module> TINO_DP_ON
 * to activate partial debugging in a certain module
 * (this overrides other defines)
 *
 * $Log$
 * Revision 1.3  2005-10-30 03:23:52  tino
 * See ChangeLog
 *
 * Revision 1.2  2004/04/06 04:41:37  tino
 * xml reader
 *
 * Revision 1.1  2004/03/28 01:24:14  tino
 * some more stuff
 */

#ifndef tino_INC_debug_h
#define tino_INC_debug_h

#include <stdio.h>
#include <stdarg.h>

#ifndef	DP
#define	DP	TINO_DP_main
#endif

#ifndef xDP
#define	xDP	TINO_DP_all
#endif

#ifndef TINO_DP_main
#define	TINO_DP_main	TINO_DP_ON
#endif

#ifndef TINO_DP_all
#define	TINO_DP_all	TINO_DP_OFF
#endif

#ifndef	TINO_DP_proc
#define TINO_DP_proc	TINO_DP_all
#endif

#define TINO_DP_ON(X)	do { tino_debugprintf X; } while (0)
#define TINO_DP_OFF(X)	do { ; } while (0)

static void
tino_debugprintf(const char *s, ...)
{
  va_list	list;

  fflush(stdout);
  fprintf(stderr, "[");
  va_start(list, s);
  vfprintf(stderr, s, list);
  va_end(list);
  fprintf(stderr, "]\n");
  fflush(stderr);
}

#endif
