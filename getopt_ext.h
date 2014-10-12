/* UNIT TEST FAILS *
 * NOT READY YET!
 *
 * Extended options
 *
 * Copyright (C)2009-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_getopt_ext_h
#define tino_INC_getopt_ext_h

#define	TINO_GETOPT_EXT		"ext\1"
#ifndef TINO_GETOPT_EXT_MAX
#define	TINO_GETOPT_EXT_MAX	100
#endif

#include "getopt.h"

#if 0
/* This is used with TINO_GETOPT_EXT setting, here an example:
 */

static void *
extended_param(void)
{
  return tino_getopt_ext(TINO_GETOPT_HELP	/* this first option is mandatory!	*/
			 "help	print this help"
			 ,

			 TINO_GETOPT_STRING
			 "string=content	help: set string_var to content"
			 , &string_var,

			 NULL);
}

int
main(int argc, char **argv)
{
  ...;
  argn	= tino_getopt(argc, argv,
		      ...,

		      TINO_GETOPT_EXT
		      "opt arg	help"
		      , extended_param()

		      ...
		      NULL);
  ...;
}
#endif

static void *
tino_getopt_ext(const char *s, ...)
{
  struct tino_getopt_impl	*p;
  int				n;

  p	= tino_alloc0o((TINO_GETOPT_EXT_MAX+1)*sizeof *p);
  tino_getopt_init(&list, p+1, TINO_GETOPT_EXT_MAX)
  p	= tino_realloc(p, 
  return p;
}

#endif
