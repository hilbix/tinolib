/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-03-23 21:35:08  tino
 * error, verror, vex
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

static void
verror(const char *prefix, const char *s, va_list list)
{
  const char	*err;

  err	= strerror(errno);
  if (prefix)
    fputs(prefix, stderr);
  vfprintf(stderr, s, list);
  fprintf(stderr, ": %s\n", err);
}

static void
error(const char *prefix, const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  verror(prefix, s, list);
  va_end(list);
}

static void
vex(const char *s, va_list list)
{
  verror("", s, list);
  exit(-1);
  abort();
  for(;;);
}

static void
ex(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  vex(s, list);
  /* never reached	*/
}
