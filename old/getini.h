/* $Header$
 *
 * Getopt and INI file processing.
 *
 * This is a getopt which also presets the values by reading from an
 * INI file.
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.1  2006-10-21 01:46:15  tino
 * Commit for save
 *
 */

#ifndef tino_INC_getopt_h
#define tino_INC_getopt_h

#include "getopt.h"

static int
tino_getini_hook(struct tino_getopt_impl *p, int max)
{
  home	= getenv("HOME");
}

static int
tino_getini(int argc, char **argv,
	    int min, int max,
	    const char *global
	    , ...
	    )
{
  va_list	list;
  int		ret;

  va_start(list, global);
  ret	= tino_getopt_hook(argc, argv, min, max, global, list, tino_getini_hook);
  va_end(list);
  return ret;
}

#endif
