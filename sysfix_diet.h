/* Various system fixes for diet
 *
 * Copyright (C)2013 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_sysfix_diet_h
#define tino_INC_sysfix_diet_h

#ifdef __dietlibc__

#ifndef	TINO_T_off_t
#define	TINO_T_off_t	off_t
#endif
#ifndef	TINO_T_fpos_t
#define	TINO_T_fpos_t	fpos_t
#endif
#ifndef	TINO_T_stat
#define	TINO_T_stat	struct stat
#endif
#ifndef	TINO_F_stat
#define	TINO_F_stat	stat
#endif
#ifndef	TINO_F_lstat
#define	TINO_F_lstat	lstat
#endif
#ifndef	TINO_F_fstat
#define	TINO_F_fstat	fstat
#endif
#ifndef	TINO_F_fopen
#define	TINO_F_fopen	fopen
#endif
#ifndef	TINO_F_freopen
#define	TINO_F_freopen	freopen
#endif
#ifndef	TINO_F_open
#define	TINO_F_open	open
#endif
#ifndef	TINO_F_ftello
#define	TINO_F_ftello	ftello
#endif
#ifndef	TINO_F_fseeko
#define	TINO_F_fseeko	fseeko
#endif
#ifndef	TINO_F_fgetpos
#define	TINO_F_fgetpos	fgetpos
#endif
#ifndef	TINO_F_fsetpos
#define	TINO_F_fsetpos	fsetpos
#endif
#ifndef	TINO_F_truncate
#define	TINO_F_truncate	truncate
#endif
#ifndef	TINO_F_ftruncate
#define	TINO_F_ftruncate	ftruncate
#endif
#ifndef	TINO_F_mmap
#define	TINO_F_mmap	mmap
#endif
#ifndef	TINO_F_lseek
#define	TINO_F_lseek	lseek
#endif

#define	posix_memalign	tino_diet_posix_memalign

#endif
#endif
