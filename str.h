/* Simple string helpers (sprintf now in strprintf.h).
 *
 * Note that tino_strdup() is in alloc.h for historic reasons.
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_str_h
#define tino_INC_str_h

#include <string.h>
#include <ctype.h>

/** Similar to strncpy,
 * but the buffer always is NUL terminated
 */
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

/** Similar to strxcpy,
 * but max gives the size of the destination,
 * which always is NUL terminated
 */
static char *
tino_strxcat(char *s, const char *src, size_t max)
{
  size_t len;

  /* Why not strlen?
   * strlen is not guaranteed to terminate, as there is no maximum length defined for it.
   */
  for (len=0; len<max && s[len]; len++);
  tino_strxcpy(s+len, src, max-len);
  return s;
}

/** Remove everything behind the given character, last match wins.
 */
static char *
tino_strrcut(char *s, char c)
{
  char	*p;

  if ((p=strrchr(s, c))!=0)
    *p	= 0;
  return s;
}

/** check if string starts with given prefix
 */
static int
tino_strprefixcmp(const char *cmp, const char *prefix)
{
  char	diff;

  while (*prefix)
    if ((diff=*cmp++-*prefix++)!=0)
      return diff;
  return 0;
}

/** check if string starts with given prefix, case insensitive
 */
static int
tino_strprefixicmp(const char *cmp, const char *prefix)
{
  char	diff;

  while (*prefix)
    if ((diff=tolower(*cmp++)-tolower(*prefix++))!=0)
      return diff;
  return 0;
}

/** startswith, which returns the position after the prefix (const)
 */
static const char *
tino_strprefixcmp2_const(const char *cmp, const char *prefix)
{
  while (*prefix)
    if ((*cmp++-*prefix++)!=0)
      return 0;
  return cmp;
}

/** startswith, which returns the position after the prefix (nonconst)
 */
static char *
tino_strprefixcmp2(char *cmp, const char *prefix)
{
  return (char *)tino_strprefixcmp2_const(cmp, prefix);
}

/** startswith case insensitive, returns the position after the prefix (const)
 */
static const char *
tino_strnprefixcmp2_const(const char *cmp, const char *prefix, size_t max)
{
  while (*prefix)
    if ((*cmp++-*prefix++)!=0)
      return 0;
  return cmp;
}

/** startswith case insensitive, returns the position after the prefix (nonconst)
 */
static char *
tino_strnprefixcmp2(char *cmp, const char *prefix, size_t max)
{
  return (char *)tino_strnprefixcmp2_const(cmp, prefix, max);
}

/** return the first position of string, which is not a space (const)
 */
static const char *
tino_str_ltrim_const(const char *s)
{
  while (*s && isspace(*s))
    s++;
  return s;
}

/** return the first position of string, which is not a space (nonconst)
 */
static char *
tino_str_ltrim(char *s)
{
  return (char *)tino_str_ltrim_const(s);
}

/** trim right hand spaces, modifying the string.
 */
static char *
tino_str_rtrim(char *s)
{
  char	*e;

  e	= s+strlen(s);
  while (e>s && isspace(*--e))
    *e	= 0;
  return s;
}

/** trim string on both sides (changes start position!)
 */
static char *
tino_str_trim(char *s)
{
  return tino_str_rtrim(tino_str_ltrim(s));
}

/** return first offset of given character, -1 when not found
 */
static int
tino_str_cpos(const char *s, char c)
{
  const char	*tmp;

  tmp	= strchr(s, c);
  if (!tmp)
    return -1;
  return tmp-s;
}

/** return length of prefix which does not contain character.
 * If string does not contain the character, length of string is returned.
 */
static int
tino_str_nclen(const char *s, char c)
{
  const char	*tmp;

  if (!s)
    return 0;
  for (tmp=s; *tmp && *tmp!=c; tmp++);
  return tmp-s;
}

/** Unescape a string with doubled escape characters only.
 *
 * This is, only the escape character can be escaped by doubling it.
 * Typically used in SQL strings to escape quote.
 *
 * Returns ptr to first character after single escape found or NULL if
 * no trailing escape found.
 *
 * MODIFIES s IN PLACE
 *
 * (Not optimally efficient yet.)
 */
static char *
tino_str_unescape_single(char *s, char escape)
{
  char	*ptr;

  if (!s)
    return 0;
  while ((ptr=strchr(s, escape))!=0)
    {
      if (ptr[1]!=escape)
        return ptr;
      s	= ptr+1;
      strcpy(ptr, s);
    }
  return 0;
}

/** Unescape a string with escape characters.
 *
 * Remove all 'escape' occurances, such that 'escape' CHAR always is
 * CHAR.  Typically used in simple grammars.
 *
 * Returns ptr to string or NULL on error (like 'escape' is the last
 * character).
 *
 * MODIFIES s IN PLACE
 *
 * (Not optimally efficient yet.)
 */
static char *
tino_str_unescape(char *s, char escape)
{
  char	*ptr, *ret	= s;

  if (!s)
    return 0;
  while ((ptr=strchr(s, escape))!=0)
    {
      if (!ptr[1])
        return 0;
      s	= ptr+1;
      strcpy(ptr, s);
    }
  return ret;
}

/** Check if the current position is a sequence of separator.  If so,
 * return the position behind the separators, else NULL.
 *
 * if sep==NULL it is any sequence of whitespaces,
 * if sep=="" it is any sequence of characters below SPC,
 * else it is the exact (single!) separator string.
 */
static const char *
tino_str_issep_const(const char *s, const char *sep)
{
  if (!sep)
    {
      if (*s && isspace(*s))
        {
          while (*++s && isspace(*s));
          return s;
        }
      return 0;
    }
  if (!*sep)
    {
      if (*s && ((unsigned char)(*s))<32)
        {
          while (*++s && ((unsigned char)(*s))<32);
          return s;
        }
      return 0;
    }
  return tino_strprefixcmp2_const(s, sep);
}

static char *
tino_str_issep(char *s, const char *sep)
{
  return (char *)tino_str_issep_const(s, sep);
}

/** Gets the next space separated argument of a string.  The next
 * position to scan is returened, the argument is at the call
 * position.
 * Changed:  You can "overcommit", it will return NULL then.
 *
 * for sep see tino_str_issep().
 *
 * if quotes==NULL no quotes exist.  Else quotes are the string-pairs
 * of quotes to look for, a missing quote is the last quote (for
 * convenience, so "'" just is ' as quotes).  Note that quotes are
 * removed(!) from the argument and can be present at any position.
 * Note that all quotes must be different, except for
 * similar-quote-pairs (like "" and '').
 *
 * If escape==NULL there is no escape.  Else it is the escape sequence
 * which escapes the next character.  There are no escapes like '\n'
 * etc.  The escape is removed from the arg, of course.
 *
 * Notes: This does not skip separators at the start of the string.
 * If you need this let the string start with a dummy argument.
 * escape overrides quotes, and both override sep.  escape is *not*
 * recognized within the first (both) quotes!  If the second quote is
 * missing any quote can terminate this quote (this makes it
 * convenient to state only the string of the single quote character).
 */
static char *
tino_str_argN(char *s, const char *sep, const char *quotes, const char *escape)
{
  char	*p;
  int	inquote;

  if (!s)
    return s;
  inquote	= 0;
  for (p=s; *s; )
    {
      char	*t;
      int	i;

      if (escape && (t=tino_strprefixcmp2(s, escape))!=0 && inquote!=1)
        s	= t;
      else if (quotes && (i=tino_str_cpos(quotes, *s))>=0)
        {
          if (inquote)
            {
              if (i==inquote || ((!quotes[inquote] || quotes[inquote]==*s) && i==inquote-1))
                {
                  inquote	= 0;
                  s++;
                  continue;
                }
            }
          else if ((i&1)==0)
            {
              inquote	= i+1;
              s++;
              continue;
            }
        }
      else if (!inquote && (t=tino_str_issep(s, sep))!=0)
        {
          s	= t;
          *p	= 0;
          return s;
        }
      *p++	= *s++;
    }
  *p	= 0;
  return 0;
}

/** convenience call to tino_str_arg()
 */
static char *
tino_str_arg_spcN(char *s)
{
  return tino_str_argN(s, NULL, NULL, NULL);
}

static int
tino_str_startswith(const char *s, const char *pref)
{
  return !tino_strprefixcmp(s, pref);
}

static const char *
tino_str_startswith_const(const char *s, const char *pref)
{
  return tino_strprefixcmp2_const(s, pref);
}

static char *
tino_str_startswith_ptr(char *s, const char *pref)
{
  return tino_strprefixcmp2(s, pref);
}

static int
tino_str_endswith_offset(const char *s, const char *suff)
{
  size_t	a, b;

  a	= strlen(s);
  b	= strlen(suff);
  if (b>a)
    return -1;
  s	+= a;
  suff	+= b;
  while (b--)
    if (*--s != *--suff)
      return -1;
  return a;
}

static int
tino_str_endswith(const char *s, const char *suff)
{
  return tino_str_endswith_offset(s, suff)>=0;
}

static const char *
tino_str_endswith_const(const char *s, const char *suff)
{
  int	pos;

  pos	= tino_str_endswith_offset(s, suff);
  return pos<0 ? 0 : s+pos;
}

static char *
tino_str_endswith_ptr(char *s, const char *suff)
{
  int	pos;

  pos	= tino_str_endswith_offset(s, suff);
  return pos<0 ? 0 : s+pos;
}

#endif

