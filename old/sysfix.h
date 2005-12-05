/* $Header$
 *
 * Various system fixes I don't want to do all the time again and again
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.2  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.1  2004/07/05 23:52:19  tino
 * added
 */

#ifndef tino_INC_sysfix_h
#define tino_INC_sysfix_h

/* Some define it as __va_copy (pre C99)
 * or va_copy (C99)
 */
#ifdef __va_copy
#ifndef va_copy
#define TINO_VA_COPY	__va_copy
#endif
#endif
#ifndef TINO_VA_COPY
#define TINO_VA_COPY	va_copy
#endif

#endif
