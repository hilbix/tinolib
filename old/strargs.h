/* $Header$
 *
 * String argument lists (argc/argv style)
 *
 * Copyright (C)2010 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.1  2010-01-25 22:57:27  tino
 * Changes for socklinger
 *
 */

#ifndef tino_INC_str_args_h
#define tino_INC_str_args_h

#include "str.h"
#include "strprintf.h"

#define	cDP	TINO_DP_str

typedef struct tino_str_args
  {
    int		argc;
    char	**argv;
    char	*line, *end, *next;
    const char	*sep, *quote, *escape;
  } tino_str_args;

/** Split a string into the whitespace separated list (like a
 * commandline).  It allows ' or " as quotes and \ as escape.
 *
 * Note that splitting is not done here, it is done on demand.
 */
static tino_str_args *
tino_str_args_init(tino_str_args *arg, const char *str)
{
  cDP(("(%p,%s)", arg, str));
  arg->argc	= 0;
  arg->argv	= 0;
  arg->line	= tino_strdupO(str);
  arg->next	= tino_str_trim(arg->line);
  arg->end	= arg->line;
  while (*arg->end++);
  arg->sep	= NULL;
  arg->quote	= "\'\'\"\"";
  arg->escape	= "\\";
  return arg;
}

static void
tino_str_args_free(tino_str_args *arg)
{
  int	i;

  cDP(("(%p)", arg));
  for (i=arg->argc; --i>=0; )
    {
      const char	*ptr = arg->argv[i];

      arg->argv[i]	= 0;
      /* free the argument if it is not within the line	*/
      if (ptr && (ptr<arg->line || ptr>=arg->end))
	{
	  cDP(("() kill %d=%s", i, ptr));
	  tino_free_constO(ptr);
	}
    }
  TINO_FREE_NULL(arg->argv);
  arg->argc	= 0;
  arg->next	= 0;
  TINO_FREE_NULL(arg->line);
}

/** Get the next escaped argument
 */
static char *
tino_str_args_next(tino_str_args *arg)
{
  char	*tmp;

  tmp	= arg->next;
  if (tmp)
    arg->next	= tino_str_arg(arg->next, arg->sep, arg->quote, arg->escape);
  cDP(("(%p) %s", arg, tmp));
  return tmp;
}

/** Append an argument.  Str must be malloc()ed.
 *
 * This returns the string for easy NULL checking.
 *
 * Well, yes, this could be optimized
 */
static char *
tino_str_args_add(tino_str_args *arg, char *str)
{
  cDP(("(%p,%s)", arg, str));
  arg->argc++;
  arg->argv			= tino_reallocO(arg->argv, arg->argc*sizeof *arg->argv);
  arg->argv[arg->argc-1]	= str;
  return str;
}

/** Get the rest of the line as an argument list (like argc/argv).
 */
static char * const *
tino_str_args_argv(tino_str_args *arg)
{
  char	*tmp;

  cDP(("(%p)", arg));
  while (*(tmp=tino_str_args_next(arg))!=0)
    tino_str_args_add(arg, tmp);
  tino_str_args_add(arg, NULL);
  cDP(("() ret %p", arg->argv));
  return arg->argv;
}

/** Get environment strings (VAR=args)
 *
 * Name unnamed env vars UNKNOWN1 to UNKNOWNn according to unknown_prefix
 */
static char * const *
tino_str_args_env(tino_str_args *arg, const char *unknown_prefix)
{
  char	*tmp;
  int	nr=0;

  cDP(("(%p,%s)", arg, unknown_prefix));
  while (*(tmp=tino_str_args_next(arg))!=0)
    tino_str_args_add(arg,
		      strchr(tmp,'=')
		      ? tmp
		      : tino_str_printf("%s%d=%s", unknown_prefix, ++nr, tmp));
  tino_str_args_add(arg, NULL);
  cDP(("() ret %p", arg->argv));
  return arg->argv;
}

#undef cDP
#endif
