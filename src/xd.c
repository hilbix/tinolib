/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-02-14 20:27:09  tino
 * Initial add
 *
 */

#include "uni2prn.c"

/*IMP*/

static void
tino_xd(unsigned long pos, const unsigned char *p, int len)
{
  int	i;

  if (!p || !len)
    {
      printf("\t%04lu:\n", pos);
      return;
    }
  for (i=0; i<len; i+=16)
    {
      int	j;

      printf("\t%04lu:", pos+i);
      for (j=0; j<16 && i+j<len; j++)
	printf(" %02x", p[i+j]);
      while (j<16 && i+j<len)
	printf("   ");
      printf(" ! ");
      for (j=0; j<16 && i+j<len; j++)
	printf("%c", tino_uni2prn(p[i+j]));
      printf("\n");
    }
}
