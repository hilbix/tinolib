/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-03-26 20:06:37  tino
 * dirty mode and name fixes
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 *
 */

#define	tino_FATAL(X)	do { if (X) { tino_fatal(#X, __FILE__, __LINE__); } } while(0)

static void
tino_fatal(const char *s, const char *file, int line)
{
  fprintf(stderr, "fatal error: %s:%d: %s\n", file, line, s);
  exit(-2);
  abort();
  for(;;);
}
