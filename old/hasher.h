/* $Header$
 *
 * This function shall have following probabilities:
 * A==B: for all nr: fn(nr,A)==fn(nr,B)
 * A!=B:(1) there exists a nr for which fn(nr,A)!=fn(nr,B)
 * A!=B:(2) fn(nr,A)==fn(nr,B) then with high probability fn(nr+1,A)!=fn(nr+1,B)
 * Well, we could use a crypto function for this, however this is slow.
 *
 * $Log$
 * Revision 1.4  2005-06-04 14:08:09  tino
 * include plus minor fix
 *
 * Revision 1.3  2005/03/04 00:58:56  tino
 * Old hash function was poor
 *
 * Revision 1.2  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/04/20 23:51:38  tino
 * Hashing added (untested!)
 */

#ifndef tino_INC_hasher_h
#define tino_INC_hasher_h

#include <stdio.h>

/* Well, I would like to do
 * sum*=n
 * sum+=ptr[i]
 * as this *must* fulfill what I wrote above (mathematically, not numerically).
 * But this is dead slow (multiplication).
 */
static long
hasher(const unsigned char *ptr, size_t len, int nr)
{
  long	sum;
  int	i;

  if (!nr)
    return (((int)ptr[0])<<8)|ptr[len-1];
  sum	= 0;
  for (i=len; --i>=0; )
    {
#if 0
      sum	+= nr;
      sum	^= ptr[i];
#else
      sum	*= nr;
      sum	+= ptr[i];
#endif
    }
  return sum;
}

#endif
