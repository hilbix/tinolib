/* $Header$
 *
 * Simple string helpers (sprintf now in strprintf.h).
 *
 * Note that tino_strdup() is in alloc.h for historic reasons.
 *
 * Copyright (C)2004-2006 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.17  2007-04-08 10:26:32  tino
 * tino_str_unescape
 *
 * Revision 1.16  2006/03/18 04:32:26  tino
 * tino_strnprefixcmp2 added
 *
 * Revision 1.15  2006/01/29 17:52:13  tino
 * part of str.h splitted into strprintf.h
 *
 * Revision 1.14  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.13  2005/12/04 05:38:03  tino
 * strprefixicmp
 *
 * Revision 1.12  2005/12/03 12:48:58  tino
 * const version added in prefixcmp
 *
 * Revision 1.11  2005/10/30 03:23:52  tino
 * See ChangeLog
 *
 * Revision 1.10  2005/08/02 04:44:41  tino
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

#include <string.h>
#include <ctype.h>

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

static int
tino_strprefixicmp(const char *cmp, const char *prefix)
{
  char	diff;

  while (*prefix)
    if ((diff=tolower(*cmp++)-tolower(*prefix++))!=0)
      return diff;
  return 0;
}

static const char *
tino_strprefixcmp2_const(const char *cmp, const char *prefix)
{
  while (*prefix)
    if ((*cmp++-*prefix++)!=0)
      return 0;
  return cmp;
}

static char *
tino_strprefixcmp2(char *cmp, const char *prefix)
{
  return (char *)tino_strprefixcmp2_const(cmp, prefix);
}

static const char *
tino_strnprefixcmp2_const(const char *cmp, const char *prefix, size_t max)
{
  while (*prefix)
    if ((*cmp++-*prefix++)!=0)
      return 0;
  return cmp;
}

static char *
tino_strnprefixcmp2(char *cmp, const char *prefix, size_t max)
{
  return (char *)tino_strnprefixcmp2_const(cmp, prefix, max);
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

#endif
