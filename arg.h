/* For some reasons 64 bit systems do not like pointers to va_list.
 * But I need this.  So here are the compatible routines.
 *
 * Copyright (C)2006-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef	tino_INC_arg_h
#define	tino_INC_arg_h

#include <stdio.h>
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
    const char	*str;
    va_list	list;
  } tino_va_list, *TINO_VA_LIST;

#define	tino_va_start(List,arg)	do { (List).str=(arg); va_start((List).list,arg); } while (0)
#define	tino_va_arg(List,type)	va_arg((List).list,type)
#define	tino_va_end(List)	va_end((List).list)
#define	tino_va_copy(A,B)	do { (A).str=(B).str; TINO_VA_COPY_SYS((A).list,(B).list); } while (0)
#define tino_va_init(A,B,arg)	do { (A).str=(arg); TINO_VA_COPY_SYS((A).list,(B)); } while (0)
#define	tino_va_get(List)	((List).list)
#define	tino_va_str(List)	((List).str)	/* this is assignable!	*/

#define	TINO_VA_ARG(List,type)	tino_va_arg(*(List),type)
#define	TINO_VA_GET(List)	tino_va_get(*(List))
#define	TINO_VA_STR(List)	((List)->str)

/* Argument clean functions: No sideeffects to the va_list
 */

static void
tino_vfprintf(FILE *fd, TINO_VA_LIST list)
{
  tino_va_list	list2;

  tino_va_copy(list2, *list);
  vfprintf(fd, tino_va_str(list2), tino_va_get(list2));
  tino_va_end(list2);
}

static int
tino_vsnprintf(char *buf, size_t max, TINO_VA_LIST list)
{
  tino_va_list	list2;
  int		n;

  tino_va_copy(list2, *list);
  n	= vsnprintf(buf, max, tino_va_str(list2), tino_va_get(list2));
  tino_va_end(list2);
  if (max>0)
    buf[max-1]	= 0;
  return n;
}

/* Use this as example prototype (I am always missing this, too)
 */
static void
tino_fprintf(FILE *fd, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vfprintf(fd, &list);
  tino_va_end(list);
}

static int
tino_snprintf(char *buf, size_t max, const char *s, ...)
{
  tino_va_list	list;
  int		n;

  tino_va_start(list, s);
  n	= tino_vsnprintf(buf, max, &list);
  tino_va_end(list);
  return n;
}

static const char *
tino_snprintf_ret(char *buf, size_t max, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vsnprintf(buf, max, &list);
  tino_va_end(list);
  return buf;
}

#endif
