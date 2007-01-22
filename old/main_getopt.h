/* $Header$
 *
 * Standard type main programs: Main with getopt
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.2  2007-01-22 19:04:22  tino
 * User-arg added to hook
 *
 * Revision 1.1  2006/12/12 13:25:05  tino
 * main_getopt.h added
 *
 */

#ifndef tino_INC_main_getopt_h
#define tino_INC_main_getopt_h

#include "main.h"
#include "getopt.h"

/* Simple program with command line options but no args
 *
 * Getopt-Vars are global!
 */
static int
tino_main_g0(void (*real_main)(void),
	     int argc, char **argv,      /* argc,argv as in main */
	     const char *global          /* string of global settings    */
	     /* append the general commandline usage to global (with a SPC) */
	     , ...)
{
  tino_va_list	list;
  int		argn;

  tino_va_start(list, global);
  argn	= tino_getopt_hook(argc, argv, 0, 0, global, &list, NULL, NULL);
  tino_va_end(list);

  if (argn<=0)
    return 1;

  real_main();
  return tino_main_errflag ? 2 : 0;
}

#endif
