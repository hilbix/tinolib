/* $Header$
 *
 * Shell pattern search in memory areas.
 *
 * Based on strwild.h.
 *
 * Copyright (C)2004 by Valentin Hilbig
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.2  2004-09-04 20:16:38  tino
 * should work now
 *
 * Revision 1.1  2004/08/18 16:02:07  Administrator
 * working on it
 */

#ifndef tino_INC_memwild_h
#define tino_INC_memwild_h

#include "fatal.h"

static int
tino_memwild(const void *_str, size_t slen, const void *_wild, size_t wlen,
	     int allq, int existq, int openq, int closeq)
{
  const unsigned char	*str=_str, *wild=_wild;
  int			pos, w, s;

  xDP(("tino_memwild(%p,%ld,%p,%ld,%d,%d,%d,%d)",
      _str, slen, _wild, wlen,
      allq, existq, openq, closeq));
  if (closeq<0)
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
  for (pos=-1, w=0, s=0; w<wlen; pos=s)
    {
      int	cmp;

      for (cmp=w; wild[cmp]!=allq; cmp++, s++)
	{
	again:
	  xDP(("cmp=%d pos=%d w=%d s=%d", cmp, pos, w, s));
	  /* When we are at the end of the string we either have a
	   * match (end of wildcard) or we ran out of compareable
	   * bytes, this is a mismatch
	   */
	  if (s>=slen)
	    {
	      xDP(("end %s", cmp>=wlen ? "match" : "nomatch"));
	      return cmp>=wlen ? 0 : 1;
	    }
	  if (cmp<wlen)
	    {
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
		    {
		      xDP(("variant error"));
		      return -1;
		    }
		  if (wild[cmp]=='^')
		    {
		      cmp++;
		      rev	= 1;
		    }
		  do
		    {
		      if (++cmp>=wlen)
			{
			  xDP(("variant error"));
			  return -1;
			}
		      if (wild[cmp]=='-')
			{
			  unsigned char	from, to;

			  from	= wild[cmp-1];
			  if (++cmp>=wlen)
			    {
			      xDP(("variant error"));
			      return -1;
			    }
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
	    }

	  /* We reach this here in following cases:
	   * 1) End of wildcard string reached but not end of string
	   * 2) Mismatch at current position
	   */
	  if (pos<0)
	    {
	      xDP(("no star"));
	      return 1;	/* no previous *, no match	*/
	    }
#if 1
	  cmp	= w;
	  s	= ++pos;
	  goto again;	/* speedup	*/
#else
	  cmp	= w-1;
	  s	= pos++;
#endif
	}
      /* we have a *
       * remember the position
       * save 'rescan' position (in loop)
       */
      w	= cmp+1;
    }
  xDP(("match"));
  return 0;	/* trailing * match (or empty wildcard matches empty string) */
}

static int
tino_memwildcmp(const void *s, size_t slen, const void *wild, size_t wlen)
{
  return tino_memwild(s, slen, wild, wlen, '*', '?', '[', ']');
}

#endif
