/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-02-14 20:27:09  tino
 * Initial add
 *
 */

#define FATAL(X)	do { if (X) debug_fatal(#X , __FILE__, __LINE__); } while (0)

/*IMP*/

static void
fatal(const char *s, ...)
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
debug_fatal(const char *s, const char *file, int line)
{
  fatal("%s:%d: condition %s\n", file, line, s);
}
