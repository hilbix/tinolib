/* $Header$
 *
 * $Log$
 * Revision 1.6  2004-10-16 21:48:56  tino
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

#endif
