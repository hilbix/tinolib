/* $Header$
 *
 * $Log$
 * Revision 1.10  2005-08-02 04:44:41  tino
 * C++ changes
 *
 * Revision 1.9  2005/04/24 12:55:38  tino
 * started GAT support and filetool added
 *
 * Revision 1.8  2005/01/26 10:51:57  tino
 * Changes for updated exception.h
 *
 * Revision 1.7  2005/01/25 22:10:19  tino
 * tino_str_vprintf added
 *
 * Revision 1.6  2004/10/16 21:48:56  tino
 * dev.h enabled, tino_trim added
 *
 * Revision 1.5  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.4  2004/06/17 21:08:27  tino
 * prefixcmp2 added
 *
 * Revision 1.3  2004/05/01 01:35:09  tino
 * new function strrcut
 *
 * Revision 1.2  2004/04/30 21:28:31  tino
 * moved strprefixcmp from strwild.h to str.h
 *
 * Revision 1.1  2004/04/29 22:37:15  tino
 * new convenience functions
 */

#ifndef tino_INC_str_h
#define tino_INC_str_h

#include <ctype.h>
#include "fatal.h"

static char *
tino_strxcpy(char *s, const char *src, size_t max)
{
  if (max)
    {
      strncpy(s, src, max);
      s[max-1]	= 0;
    }
  return s;
}

static char *
tino_strxcat(char *s, const char *src, size_t max)
{
  size_t len;

  /* Why not strlen?
   * strlen is not guaranteed to terminated, as there is no maximum length defined for it.
   */
  for (len=0; len<max && s[len]; len++);
  tino_strxcpy(s+len, src, max-len);
  return s;
}

static char *
tino_strrcut(char *s, char c)
{
  char	*p;

  if ((p=strrchr(s, c))!=0)
    *p	= 0;
  return s;
}

static int
tino_strprefixcmp(const char *cmp, const char *prefix)
{
  char	diff;

  while (*prefix)
    if ((diff=*cmp++-*prefix++)!=0)
      return diff;
  return 0;
}

static const char *
tino_strprefixcmp2(const char *cmp, const char *prefix)
{
  while (*prefix)
    if ((*cmp++-*prefix++)!=0)
      return 0;
  return cmp;
}

static char *
tino_trim(char *s)
{
  const char	*e;

  while (isspace(*s))
    s++;
  e	= s+strlen(s);
  while (e>s && isspace(*--e));
  return s;
}

/***********************************************************************/

static char *
tino_str_vprintf_null(const char *s, va_list orig)
{
  int	n;

  n	= BUFSIZ;
  for (;;)
    {
      va_list	list;
      char	*tmp;
      int	k;

      tmp	= (char *)malloc(n);
      if (!tmp)
	return 0;

      va_copy(list, orig);
      k	= vsnprintf(tmp, n, s, list);
      va_end(list);
      tino_FATAL(k<0);
      if (++k<=n)
	{
	  realloc(tmp, k);
	  return tmp;
	}
      free(tmp);
      /* There is a bug in older libraries.
       * vsnprintf does not return the size needed,
       * instead it returns max.
       * We cannot distinguish between this case and the case,
       * that just the space for the \0 is missing.
       * Therefor we define to always extend buffer by BUFSIZ at minimum.
       */
      k	-= n;
      if (k<BUFSIZ)
	k	= BUFSIZ;
      n	+= k;
    }
}

static char *
tino_str_vprintf(const char *s, va_list orig)
{
  char	*tmp;

  tmp	= tino_str_vprintf_null(s, orig);
  if (!tmp)
    TINO_FATAL(("out of memory allocating string for %s", s));
  return tmp;
}


#endif
