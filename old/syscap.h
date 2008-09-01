/* $Header$
 *
 * Various system capabilities which cannot be detected in sysfix.h
 * (Keep it in sysfix.h if possible!)
 *
 * Never prototype anything.  Just #define!
 *
 * All other includes shall depend on the defines detected here.
 * (Note that you must include everything first for this here to
 * detect.  This does not include anything!)
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
 * Revision 1.2  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.1  2007-01-28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 */

#ifndef tino_INC_syscap_h
#define tino_INC_syscap_h

#ifndef	TINO_HAS_IPv6
#ifdef	IPPROTO_IPV6
#define	TINO_HAS_IPv6
#endif
#endif
#ifdef	TINO_NO_IPV6
#undef	TINO_HAS_IPv6
#endif

#endif
