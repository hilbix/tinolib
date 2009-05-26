/* $Header$
 *
 * Must be included *first*.
 * defines all the "dirty" names I use for my routines.
 * Note that, this way, you can use both, dirty and non-dirty names.
 *
 * Copyright (C)2004-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.11  2009-05-26 19:41:57  tino
 * strxcpy
 *
 * Revision 1.10  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.9  2006-02-11 14:36:11  tino
 * 000; is now TINO_XXX;
 *
 * Revision 1.8  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.7  2004/10/16 21:48:56  tino
 * dev.h enabled, tino_trim added
 *
 * Revision 1.6  2004/04/13 00:29:12  tino
 * A lot of changes to do compile.  Far from ready yet.
 *
 * Revision 1.5  2004/04/07 02:22:48  tino
 * Prototype for storing data in gff_lib done (untested)
 *
 * Revision 1.4  2004/03/28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.3  2004/03/26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.1  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
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
#define	tino_err	error
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
