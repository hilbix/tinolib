/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-02-14 20:27:09  tino
 * Initial add
 *
 */

static int
tino_uni2prn(unsigned long c)
{
  if (c>0xff)
    return '?';
  if (c<0x20 || (c>=0x7f && c<0xa0))
    return '.';
  return c;
}
