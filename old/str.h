/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-04-30 21:28:31  tino
 * moved strprefixcmp from strwild.h to str.h
 *
 * Revision 1.1  2004/04/29 22:37:15  tino
 * new convenience functions
 *
 */

#ifndef tino_INC_str_h
#define tino_INC_str_h

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
  strxcpy(s+len, src, max-len);
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

#endif
