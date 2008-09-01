/* $Header$
 *
 * Standard type main programs: Main with getini and files
 *
 * Copyright (C)2007-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.4  2008-05-07 15:02:30  tino
 * 0 arg functions
 *
 * Revision 1.3  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.2  2007/01/25 04:40:49  tino
 * Improvements in getopt and standard "main" routines (error-behavior).
 * getopt not yet completely ready, commit because this here works again (mostly).
 *
 * Revision 1.1  2007/01/22 19:05:55  tino
 * tino_main_if added and getini now works as a dummy
 */

#ifndef tino_INC_main_getini_h
#define tino_INC_main_getini_h

#include "main.h"
#include "getini.h"

/* Simple program with command line options and easy to process args
 */
static int
tino_main_if(void (*real_main)(const char *, void *),
	     void *user,
	     int *errflag,
	     int argc, char **argv,      /* argc,argv as in main */
	     int min, int max,
	     const char *global          /* string of global settings    */
	     /* append the general commandline usage to global (with a SPC) */
	     , ...)
{
  tino_va_list	list;
  int		argn, err;

  tino_main_set_error(&err, errflag);

  tino_va_start(list, global);
  argn	= tino_getini_varg(argc, argv, min, max, &list);
  tino_va_end(list);

  if (argn<=0)
    return 1;

  if (min==0 && argn>=argc)
    real_main(NULL, user);
  while (argn<argc)
    real_main(argv[argn++], user);
  return tino_main_get_error(err);
}

#endif
