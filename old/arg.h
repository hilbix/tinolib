/* $Header$
 *
 * For some reasons 64 bit systems do not like pointers to va_list.
 * But I need this.  So here are the compatible routines.
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
 * Revision 1.1  2006-10-04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 */

#ifndef	tino_INC_arg_h
#define	tino_INC_arg_h

#include <stdarg.h>

/* Some define it as __va_copy (pre C99)
 * or va_copy (C99)
 */
#ifdef	__va_copy
#ifndef	va_copy
#define	TINO_VA_COPY_SYS	__va_copy
#endif
#endif
#ifndef	TINO_VA_COPY_SYS
#define	TINO_VA_COPY_SYS	va_copy
#endif

typedef struct tino_varg
  {
    va_list	list;
  } tino_va_list, *TINO_VA_LIST;

#define	tino_va_start(List,arg)	va_start((List).list,arg)
#define	tino_va_arg(List,type)	va_arg((List).list,type)
#define	tino_va_end(List)	va_end((List).list)
#define	tino_va_copy(A,B)	TINO_VA_COPY_SYS((A).list,(B).list)
#define	tino_va_get(List)	((List).list)

#define	TINO_VA_ARG(List,type)	tino_va_arg(*List,type)
#define	TINO_VA_GET(List)	tino_va_get(*List)

#endif
