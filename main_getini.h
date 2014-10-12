/* Standard type main programs: Main with getini and files
 *
 * Copyright (C)2007-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
