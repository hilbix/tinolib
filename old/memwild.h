/* $Header$
 *
 * Shell pattern search in memory areas.
 *
 * Based on strwild.h (from tinolib), this will become part of tinolib soon.
 * Note that tinolib is not distributed, it's always part of other programs, like md5backup.
 *
 * Copyright (C)2004 by Valentin Hilbig
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.1  2004-08-18 16:02:07  Administrator
 * working on it
 *
 */

static int
tino_memwild(const void *_str, size_t slen, const void *_wild, size_t wlen,
	     int allq, int existq, int openq, int closeq)
{
  const unsigned char	*str=_str, *wild=_wild;
  int			pos, w, s;

  if (!closeq)
    closeq	= openq;
  if (!str || !wild)
    return -1;		/* report error	(NULL) */
#if 1
  /* Beware: "a[x]" does not match "a[x]"
   * in case you interpret '[x]' equivalent to 'x'
   */
  if (str==wild)	/* any wildcard matches itself	*/
    return 0;
#endif
  if (!wlen && slen)	/* special case: empty wildcard matches empty string */
    return 1;
  for (pos=0, w=0, s=0; w<wlen; pos=s)
    {
      int	cmp;

      for (cmp=w; wild[cmp]!=allq; cmp++, s++)
	{
	again:
	  if (wild[cmp]==openq && openq)
	    {
	      int	rev, match;

	      /* We then can match anything if this is defined
	       * First match is for close character -> literally
	       * So we have:
	       * [[] is [	[?] is ?
	       * []] is ]	[*] is *
	       * [][] is ] or [	[^-] is anything but -
	       * You can use any character instead of [ and ] i. E. \:
	       * \?\ is ?	\*\ is *
	       * \\\ is \ (funny)
	       * \\*\ is \ or *
	       */
	      rev	= 0;
	      match	= 0;
	      if (++cmp>=wlen)
		return -1;
	      if (wild[cmp]=='^')
		{
		  cmp++;
		  rev	= 1;
		}
	      do
		{
		  if (++cmp>=wlen)
		    return -1;
		  if (wild[cmp]=='-')
		    {
		      unsigned char	from, to;

		      from	= wild[cmp-1];
		      if (++cmp>=wlen)
			return -1;
		      if (from>(to=wild[cmp]))
			{
			  if (from>str[s] && str[s]>to)
			    match	= 1;
			}
		      else if (from<=str[s] && str[s]<=to)
			match	= 1;
		    }
		  else if (str[s]==wild[cmp-1])
		    match	= 1;
		} while (wild[cmp]!=closeq);
	      if (match!=rev)
		continue;
	      /* no match, fallthrough	*/
	    }
	  else if (str[s]==wild[cmp] || wild[cmp]==existq)
	    continue;		/* match	*/

	  /* Other cases left from above:
	   * *cmp==0 and *s!=0	-> no match yet, advance last *
	   * *cmp!=0 and *s!=0	-> different, advance last *
	   */
	  if (!pos)
	    return 1;	/* no previous *, no match	*/

#if 1
	  cmp	= w;
	  s	= ++pos;
	  goto again;	/* speedup	*/
#else
	  cmp	= w-1;
	  s	= pos++;
#endif
	}
      if (allq)
	return ( ? 1 : 0)	/* funny special case: no allquantor	*/
      /* we have a *
       * remember the position
       * save 'rescan' position (in loop)
       */
      w	= cmp+1;
    }
  return 0;	/* trailing * match (or empty wildcard matches empty string) */
}

static int
tino_memwildcmp(const void *s, size_t slen, const void *wild, size_t wlen)
{
  return tino_memwild(s, slen, wild, wlen, '*', '?', '[', ']');
}
