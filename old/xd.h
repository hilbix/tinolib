/* $Header$
 *
 * $Log$
 * Revision 1.3  2004-03-28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.1  2004/03/26 19:58:04  tino
 * added
 */

#ifndef tino_INC_xd_h
#define tino_INC_xd_h

static int
tino_uni2prn(unsigned c)
{
  if (c>0xff)
    return '?';
  if (c<0x20 || (c>=0x7f && c<0xa0))
    return '.';
  return c;
}

static void
tino_xd(FILE *fd, unsigned long pos, const unsigned char *p, int len)
{
  int	i;

  if (!p || !len)
    {
      fprintf(fd, "\t%04lu:\n", pos);
      return;
    }
  for (i=0; i<len; i+=16)
    {
      int	j;

      fprintf(fd, "\t%04lu:", pos+i);
      for (j=0; j<16 && i+j<len; j++)
	fprintf(fd, " %02x", p[i+j]);
      while (j<16 && i+j<len)
	fprintf(fd, "   ");
      fprintf(fd, " ! ");
      for (j=0; j<16 && i+j<len; j++)
	fprintf(fd, "%c", tino_uni2prn(p[i+j]));
      fprintf(fd, "\n");
    }
}

#endif
