/* Standard type main programs: Main with getopt and parameters
 *
 * Copyright (C)2011-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_main_getext_h
#define tino_INC_main_getext_h

#include "main.h"
#include "getopt.h"

/* like tino_main_g0(), but with passing of the remaining parameters
 *
 * Getopt-Vars are global!
 */
static int
tino_main_g1(void (*real_main)(const char *arg0, int argc, char **argv),
	     int *errflag,
	     int argc, char **argv,      /* argc,argv as in main */
	     int min, int max,
	     const char *global          /* string of global settings    */
	     /* append the general commandline usage to global (with a SPC) */
	     , ...)
{
  tino_va_list	list;
  int		argn;
  int		err;

  tino_main_set_error(&err, errflag);

  tino_va_start(list, global);
  argn	= tino_getopt_hook(argc, argv, min, max, &list, NULL, NULL);
  tino_va_end(list);

  if (argn<=0)
    return 1;

  /* warning, this is not really compatible to main(argc,argv)
   * as args start at 0 not 1
   */
  real_main(argv[0], argc-argn, argv+argn);
  return tino_main_get_error(err);
}

#endif
