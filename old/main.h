/* $Header$
 *
 * Standard type main programs
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
 * Revision 1.3  2007-01-25 04:40:49  tino
 * Improvements in getopt and standard "main" routines (error-behavior).
 * getopt not yet completely ready, commit because this here works again (mostly).
 *
 * Revision 1.2  2006/10/04 02:29:10  tino
 * More tino_va_*
 *
 * Revision 1.1  2006/09/28 01:54:10  tino
 * added
 */

#ifndef tino_INC_main_h
#define tino_INC_main_h

#include "ex.h"
#include "buf_line.h"

/* Note about my "upcomming return value standard" for programs:
 * 0		OK
 * 1		Usage and parameter problems
 * 2		General error
 * 3..9		reserved for future standard errors/conditions
 * 10 to 63	application defined conditions
 * 64 to 99	application defined anything (conditions/errors)
 * 100 to 127	application defined errors
 * 128 to 199	application defined anything (errors/fatals)
 * 200 to 239	application defined fatals
 * 240..254	reserved for future standard fatals
 * 255		Unspecified type of error
 */

/* Error flag given from initialization:
 *
 * NULL is standard behavior (like =0)
 * <0 errors are fatal, terminate program with -value
 * =0 standard behavior: ignore errors, return 2 on error, else 0
 * >0 ignore errors, return 2 on error, else 0
 *
 * The error flag value is set to 0 after initialization if >0.  You
 * can change it then, but the main() return value always is the one
 * from the initialization. (If you set it <0 later you inidicate,
 * that errors shall not be ignored.)
 *
 * Following now is standard, and you can rely on this fact:
 * errflag is <=0 while everything is OK.
 * errflag is 3 while we are in the error routine.
 * errflag is 2 when new errors were seen.
 * All other values are for your own purpose.
 *
 * This way you can have cascaded error handlers which can detect if
 * the "inner" part encountered an error.  Just grab an unique value
 * >3 where lower numbers are "inner".  If you see something >0 and
 * <value you set the flag to value knowing that some inner routine
 * encountered an error.
 */
static int *tino_main_errflag;

static void
tino_main_verror_fn(const char *prefix, const char *s, TINO_VA_LIST list, int err)
{
  int	tmp;

  tmp			= *tino_main_errflag;
  *tino_main_errflag	= 3;	/* We are in the error routine	*/

  tino_verror_std(prefix, s, list, err);
  if (tmp<0)
    exit(-tmp);

  /* Switch in error mode
   */
  *tino_main_errflag	= tmp ? tmp : 2;
}

/* Usage:
 * int main()
 * {
 * int err, errflag;
 * errflag	= return_value_if_ignored_errors;
 * // or
 * errflag	= -return_value_at_error;
 * tino_main_set_error(&err, &errflag);
 * ...;	// You can switch errflag from 0 to
 * ...; // -return_value_at_error any time.
 * ...;	// errflag also indicates the error state
 * ...; // it is present as *tino_main_errflag, too
 * return tino_main_get_error(err);
 * }
 */
static void
tino_main_set_error(int *err, int *errflag)
{
  if (!errflag)
    {
      errflag		= err;
      *errflag		= 0;
    }
  else if ((*err= *errflag)>=0)
    {
      *errflag		= 0;
    }
  else
    {
      *err		= - *errflag;
    }
  tino_main_errflag	= errflag;
  tino_verror_fn	= tino_main_verror_fn;
}

/* Get the return value, must be called with the error to return.
 *
 * tino_main_errflag either is a pointer to the err variable (from
 * set_error), in which case we have the default case where *err=2, or
 * tino_main_errflag is the given errflag, in this case err saved the
 * return value.
 *
 * If errflag was NULL in set_error and you directly manipulated
 * tino_main_errflag, this value is returned, as you manipulated err.
 * This is a feature.
 */
static int
tino_main_get_error(int err)
{
  if (!tino_main_errflag)
    return -1;
  if (*tino_main_errflag<=0)
    return 0;
  return err;
}

#endif
