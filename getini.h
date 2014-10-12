/* Getopt and INI file processing.
 *
 * This is a getopt which also presets the values by reading from an
 * INI file.
 *
 * Copyright (C)2006-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_getini_h
#define tino_INC_getini_h

#include "getopt.h"

static int
tino_getini_hook(struct tino_getopt_impl *p, int max, void *user)
{
  000;	/* not yet implemented	*/

#if 0
  home	= getenv("HOME");
#endif
  return 0;
}

static int
tino_getini_varg(int argc, char **argv,
		 int min, int max,
		 TINO_VA_LIST list
		 )
{
  return tino_getopt_hook(argc, argv, min, max, list, tino_getini_hook, NULL);
}

static int
tino_getini(int argc, char **argv,
	    int min, int max,
	    const char *global
	    , ...
	    )
{
  tino_va_list	list;
  int		ret;

  tino_va_start(list, global);
  ret	= tino_getini_varg(argc, argv, min, max, &list);
  tino_va_end(list);
  return ret;
}

#endif
