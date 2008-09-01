/* $Header$
 *
 * Standard type main programs: Main with getopt
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.5  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.4  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.3  2007/01/25 04:40:49  tino
 * Improvements in getopt and standard "main" routines (error-behavior).
 * getopt not yet completely ready, commit because this here works again (mostly).
 *
 * Revision 1.2  2007/01/22 19:04:22  tino
 * User-arg added to hook
 *
 * Revision 1.1  2006/12/12 13:25:05  tino
 * main_getopt.h added
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
	     int *errflag,
	     int argc, char **argv,      /* argc,argv as in main */
	     const char *global          /* string of global settings    */
	     /* append the general commandline usage to global (with a SPC) */
	     , ...)
{
  tino_va_list	list;
  int		argn;
  int		err;

  tino_main_set_error(&err, errflag);

  tino_va_start(list, global);
  argn	= tino_getopt_hook(argc, argv, 0, 0, &list, NULL, NULL);
  tino_va_end(list);

  if (argn<=0)
    return 1;

  real_main();
  return tino_main_get_error(err);
}

#endif
