/* $Header$
 *
 * CYGWIN fixes.
 *
 * This here shall combine all the fixes needed for CygWin I
 * encounter.  For some reason CygWin does not define lethal function
 * which it supports if you don't pretend that you do some internal
 * compile stuff.  However it you do this, some includes fail, as they
 * cannot be overridden.  So either you have all datatypes you need,
 * or some missing includes.
 *
 * I go gor the missing includes, as this can be fixed easily.  And
 * instructions can be issued as #warning.
 *
 * Copyright (C)2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.2  2007-03-26 17:59:11  tino
 * Bugfix: T_stat not needed this way under CygWin
 *
 * Revision 1.1  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 *
 */

#ifndef tino_INC_sysfix_cygwin_h
#define tino_INC_sysfix_cygwin_h

/* Some elder CYGWIN I believe, cannot detect any more which and when
 */
#ifndef __cygwin__
#define	TINO_NO_INCLUDE_AIO
#endif

/**********************************************************************/

#ifdef __CYGWIN__
/* Currently tested for:
 * GCC 3.4.4 gdc 0.12 mdm 0.125 (on 32 bit Windows XP)
 */
#define	TINO_NO_INCLUDE_AIO
/* I don't know why _COMPILING_NEWLIB is needed.
 */
#define _COMPILING_NEWLIB
/* _COMPILING_NEWLIB triggers some wrong include definitions in the
 * includes.  One either can have everything needed with include
 * error, or you don't have what you need.  That's perfectly
 * compatible to Microsoft ;)
 */
#warning "=================================================================="
#warning "= If getopt.h is reported missing, create an empty file getopt.h ="
#warning "= If CygWin needs fixes please try to edit tino/sysfix_cygwin.h  ="
#warning "=================================================================="

#define	__LARGE64_FILES
#define TINO_T_off_t	_off64_t
#define TINO_T_fpos_t	_fpos64_t
#define TINO_F_stat	_stat64
#define TINO_F_lstat	_lstat64
#endif

/**********************************************************************/
#endif
