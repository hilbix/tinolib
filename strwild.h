/* NOTE THAT I THINK IT STILL HAS SOME BUGS!
 *
 * Match string against wildcards, returns: 0=match, -1=error, 1=mismatch
 *
 * allquantor (*), existquantor (?) and possibilities ([...])
 * Variants:
 * First character ^ inverts content
 * First matching character can be ], so []] matches ] and [[] matches [
 * a-b matches a to b including a and b (a<=b)
 * b-a matches a to b excluding a and b (a<b)
 * How to match ] and -?  This way: []-]-] (yuck!)
 * ]-] means: ] to ] (as the - forces a "to") and then -
 *
 * You can also redefine all four characters, the [ ] can be even the same:
 * \a-zA-Z\ is possible, \\\ then matches \
 *
 * Recoursion is not needed at all:
 *
 * If we are at a *, the * can match anything.
 * So if we had a * before the *, we need never recourse to this again,
 * as the former * cannot match more than the current *,
 * as the current * already can match anything.
 * As it cannot match more, it can only match less.
 * So recoursion would be a waste, it cannot match more than we can match.
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef tino_INC_strwild_h
#define tino_INC_strwild_h

static int
tino_strwild(const char *s, const char *wild,
	     char allq, char existq, char openq, char closeq)
{
  const char	*pos;

  if (!closeq)
    closeq	= openq;
  if (!s || !wild)
    return -1;		/* report error	(NULL) */
#if 1
  /* Beware: "a[x]" does not match "a[x]"
   * in case you interpret '[x]' equivalent to 'x'
   */
  if (s==wild)		/* any wildcard matches itself	*/
    return 0;
#endif
  if (!*wild && *s)	/* special case: empty wildcard matches empty string */
    return 1;
  for (pos=0; *wild; pos=s)
    {
      const char	*cmp;

#if 0
      nextstar	= 0;
#endif
      for (cmp=wild; *cmp!=allq; cmp++, s++)
	{
	again:
	  if (*cmp==openq && openq)
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
	      if (*++cmp=='^')
		{
		  cmp++;
		  rev	= 1;
		}
	      do
		{
		  if (!*cmp)
		    return -1;
		  if (cmp[1]=='-')
		    {
		      unsigned char	from;

		      from	= *cmp;
		      cmp	+= 2;
		      if (!*cmp)
			return -1;
		      if (from>(unsigned char)*cmp)
			{
			  if (from>(unsigned char)*s
			      && (unsigned char)*s>(unsigned char)*cmp)
			    match	= 1;
			}
		      else if (from<=(unsigned char)*s
			       && (unsigned char)*s<=(unsigned char)*cmp)
			match	= 1;
		    }
		  else if (*s==*cmp)
		    match	= 1;
		} while (*++cmp!=closeq);
	      if (match!=rev)
		continue;
	      /* no match, fallthrough	*/
	    }
	  else if (*s==*cmp)
	    {
	      if (*cmp)
		continue;
	      return 0;		/* complete match	*/
	    }
	  if (!*s)	/* *cmp!=0 in this case	*/
	    return 1;	/* definitively no match	*/
	  if (*cmp==existq && existq)
	    continue;
	  /* Other cases left from above:
	   * *cmp==0 and *s!=0	-> no match yet, advance last *
	   * *cmp!=0 and *s!=0	-> different, advance last *
	   */
	  if (!pos)
	    return 1;	/* no previous *, no match	*/

#if 0
	  /* Another speedup possible here:
	   * if no * follows,
	   * do a reverse match from the end of the string,
	   * as we have something like '*whatever'
	   */
	  if (!nextstar)
	    {	
	      while (*++cmp!='*')
		if (!*cmp)
		  return trailmatch(s, wild);
	      nextstar	= cmp;
	    }
#endif
#if 1
	  cmp	= wild;
	  s	= ++pos;
	  goto again;	/* speedup	*/
#else
	  cmp	= wild-1;
	  s	= pos++;
#endif
	}
      if (!allq)
	return (*s ? 1 : 0);	/* funny special case: no allquantor	*/
      /* we have a *
       * remember the position
       * save 'rescan' position (in loop)
       */
      wild	= cmp+1;
    }
  return 0;	/* trailing * match (or empty wildcard matches empty string) */
}

static int
tino_strwildcmp(const char *s, const char *wild)
{
  return tino_strwild(s, wild, '*', '?', '[', ']');
}

#ifdef TINO_TEST_UNIT
TEST_1(tino_strwildcmp("a-b-c", "[z"));
TEST1(tino_strwildcmp("a-b-c", "*[z]*"));
TEST0(tino_strwildcmp("a-b-c", "*[b]*"));
TEST0(tino_strwildcmp("a-b-c", "*b*"));
TEST0(tino_strwildcmp("a-b-c", "*c"));
TEST0(tino_strwildcmp("a-b-c", "a????"));
TEST0(tino_strwildcmp("a-b-c", "????c"));
TEST0(tino_strwildcmp("a-b-c", "a*"));
#endif

#ifdef TINO_TEST_MAIN
#include <stdio.h>

int
main(int argc, char **argv)
{
  char	buf[BUFSIZ*10];

  while (fgets(buf, sizeof buf-1, stdin))
    {
      int	i;

      for (i=0; ++i<argc; )
	printf(" %d=%d", i, tino_strwildcmp(buf, argv[i]));
      printf("\n");
    }
  return 0;
}
#endif
#endif
