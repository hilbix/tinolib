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
 * Revision 1.4  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.3  2007-09-27 15:39:42  tino
 * CygWin fixes
 *
 * Revision 1.2  2007/03/26 17:59:11  tino
 * Bugfix: T_stat not needed this way under CygWin
 *
 * Revision 1.1  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
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
 *
 * Cygwin 1.5 has 64 bit IO by default, so disable tweaking,
 * as all the needed 64 bit routines are mostly missing.
 */
#define	TINO_NO_INCLUDE_AIO

#if 0
#warning "=================================================================="
#warning "= If getopt.h is reported missing, create an empty file getopt.h ="
#warning "= If CygWin needs fixes please try to edit tino/sysfix_cygwin.h  ="
#warning "=================================================================="
#endif

#define TINO_T_off_t	off_t
#define TINO_T_fpos_t	fpos_t
#define TINO_T_stat	struct stat
#define TINO_F_fstat	fstat
#define TINO_F_stat	stat
#define TINO_F_lstat	lstat
#define TINO_F_fopen	fopen
#define TINO_F_open	open
#define TINO_F_lseek	lseek
#define TINO_F_freopen	freopen
#define TINO_F_truncate	truncate
#define TINO_F_ftruncate	ftruncate
#define TINO_F_mmap	mmap
#define TINO_F_ftello	ftello
#define TINO_F_fseeko	fseeko
#define TINO_F_fgetpos	fgetpos
#define TINO_F_fsetpos	fsetpos
#endif

/**********************************************************************/
#endif
