/* $Header$
 *
 * This lib is not thought to be complete.
 * This lib is not thought to be elegant.
 * It's thought to be practical and easy to use.
 *
 * $Log$
 * Revision 1.1  2004-08-22 23:19:19  tino
 * sync to cvs
 *
 */

/* TINO_PUB means, this can be used publicly.
 * If you see "static" this means, it's private.
 */
#ifndef	TINO_PUB
#define TINO_PUB	static
#endif

/* TINO_IMP is a marker for "implementation ahead".
 * This will be used to extract headers from the sources.
 */
#define	TINO_IMP

#define	ex(X)	tino_ex(X)

TINO_PUB void
tino_ex(const char *s)
{
  perror(s);
  exit(3);
  abort();
  for (;;);
}
