/* $Header$
 *
 * Debugging purpose.
 * (This currently is incomplete.)
 *
 * FATAL(X) is the opposit of assert(), however with library control.
 * TINO_NO_FATAL switches off FATAL completely.
 *
 * DP((format, args)); is a debug point
 * xDP((format, args)); is a switched off debug point
 * TINO_NO_DEBUG switches off DP completely.
 *
 * $Log$
 * Revision 1.1  2004-08-22 23:19:19  tino
 * sync to cvs
 *
 */

#ifdef TINO_NO_FATAL
#define	FATAL(X)	do { ; } while(0)
#else
#define	FATAL(X)	do { if (X) { tino_fatal(#X, __FILE__, __LINE__); } } while(0)
#endif

#ifdef TINO_NO_DEBUG
#define DP(X)	do { ; } while (0)
#else
#define DP(X)	do { tino_debugprintf X; } while (0)
#endif

TINO_IMP

TINO_PUB void
tino_fatal(const char *s, const char *file, int line)
{
  fprintf(stderr, "fatal error: %s:%d: %s\n", file, line, s);
  exit(-2);
  abort();
  for(;;);
}

TINO_PUB void
tino_debugprintf(const char *s, ...)
{
  va_list list;

  fprintf(stderr, "[");
  va_start(list, s);
  vfprintf(stderr, s, list);
  va_end(list);
  fprintf(stderr, "]\n");
}
