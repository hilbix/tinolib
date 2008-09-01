/* $Header$
 *
 * Some generic data types for the case, you need types of certain width.
 * This can be easily adjusted here if needed.
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
 * Revision 1.4  2008-09-01 20:18:15  tino
 * GPL fixed
 *
 * Revision 1.3  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.2  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/04/18 14:19:49  tino
 * Sigh.  Why are there no globally universal standard data types? (K&R-C)
 */

#ifndef tino_INC_type_h
#define tino_INC_type_h

#include <sys/types.h>

/* I need some signed and unsigned types of which the storage size is
 * definitively known.
 */
typedef u_int8_t	tino_u8_t;
typedef u_int16_t	tino_u16_t;
typedef u_int32_t	tino_u32_t;
typedef u_int64_t	tino_u64_t;
typedef int8_t		tino_i8_t;
typedef int16_t		tino_i16_t;
typedef int32_t		tino_i32_t;
typedef int64_t		tino_i64_t;

#endif
