/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-03-23 21:19:51  tino
 * Scratch area
 *
 */

static void
ex(const char *s)
{
  perror(s);
  exit(-1);
  abort();
  for(;;);
}
