/* $Header$
 *
 * $Log$
 * Revision 1.4  2004-03-26 20:17:50  tino
 * More little changes
 *
 * Revision 1.3  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
 *
 * Revision 1.2  2004/03/23 21:35:08  tino
 * error, verror, vex
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

#ifndef tino_INC_ex_h
#define tino_INC_ex_h

static void
tino_verror(const char *prefix, const char *s, va_list list)
{
  const char	*err;

  err	= strerror(errno);
  if (prefix)
    fputs(prefix, stderr);
  vfprintf(stderr, s, list);
  fprintf(stderr, ": %s\n", err);
}

static void
tino_error(const char *prefix, const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_verror(prefix, s, list);
  va_end(list);
}

static void
tino_vexit(const char *s, va_list list)
{
  tino_verror(NULL, s, list);
  exit(-1);
  abort();
  for(;;);
}

static void
tino_exit(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_vexit(s, list);
  /* never reached	*/
}

#endif
