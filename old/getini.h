/* $Header$
 *
 * Getopt and INI file processing.
 *
 * This is a getopt which also presets the values by reading from an
 * INI file.
 *
 * Copyright (C)2006-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.2  2007-01-22 19:05:55  tino
 * tino_main_if added and getini now works as a dummy
 *
 * Revision 1.1  2006/10/21 01:46:15  tino
 * Commit for save
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
		 const char *global,
		 TINO_VA_LIST list
		 )
{
  return tino_getopt_hook(argc, argv, min, max, global, list, tino_getini_hook, NULL);
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
  ret	= tino_getini_varg(argc, argv, min, max, global, &list);
  tino_va_end(list);
  return ret;
}

#endif
