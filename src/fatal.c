/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-08-22 23:19:06  tino
 * just a sync to cvs
 *
 * Revision 1.1  2004/02/14 20:27:09  tino
 * Initial add
 */

#define tino_FATAL(X)	do { if (X) tino_fatal_debug(#X , __FILE__, __LINE__); } while (0)

/*IMP*/

static void
tino_fatal(const char *s, ...)
{
  va_list	list;

  fprintf(stderr, "fatal error: ");
  va_start(list, s);
  vfprintf(stderr, s, va_list);
  va_end(list, s);
  abort();
  exit(1);
  for (;;);
}

static void
tino_fatal_debug(const char *s, const char *file, int line)
{
  tino_fatal("%s:%d: condition %s\n", file, line, s);
}
