/* $Header$
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
 * $Log$
 * Revision 1.5  2004-11-23 22:28:15  tino
 * minor
 *
 * Revision 1.4  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.3  2004/08/17 23:06:58  Administrator
 * Minor (not yet used parts) bugs removed and added functions
 *
 * Revision 1.2  2004/04/30 21:28:31  tino
 * moved strprefixcmp from strwild.h to str.h
 *
 * Revision 1.1  2004/04/30 21:24:52  tino
 * added
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

#endif
