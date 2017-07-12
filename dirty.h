/* Must be included *first*.
 * defines all the "dirty" names I use for my routines.
 * Note that, this way, you can use both, dirty and non-dirty names.
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
#ifndef tino_INC_dirty_h
#define tino_INC_dirty_h

/* The right hand will become inline wrappers in future.  For now we
 * redefine the right side (which will be impossible as soon as
 * tinolib becomes a real library).
 */
#define	tino_uni2prn	uni2prn
#define	tino_alloc	alloc
#define	tino_alloc0	alloc0
#define	tino_strdup	stralloc
#define	tino_strncpy0	strxcpy
#define	tino_xd		xd
#define	tino_exit	ex
#define	tino_fatal	fatal
#ifdef TINO_NEED_OLD_ERR_FN
#define	tino_err	error
#else
#define	tino_err_new	error
#endif
#define	tino_verror	verror
#define	tino_realloc	re_alloc
#define	tino_warn	warn
#define	tino_vwarn	vwarn
#define	tino_trim	trim
#define	tino_strxcpy	strxcpy

#define	XXX		TINO_XXX

#define	FATAL(X)	tino_FATAL(X)

#define	tino_global_error_count	global_error_count

#endif
