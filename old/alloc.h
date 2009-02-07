/* $Header$
 *
 * Memory allocation routines
 *
 * all stuff arround malloc() shall go here.
 *
 * To free use free(), however THIS CAN BE A #define
 * Be sure to include this and do a full recompile (do not link only).
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
 * Revision 1.22  2009-02-07 11:09:07  tino
 * Parantheses around REALLOC macro
 *
 * Revision 1.21  2009-01-27 14:59:09  tino
 * tino_alloc_align_size_nO added
 *
 * Revision 1.20  2008-10-19 22:24:21  tino
 * tino_alloc_align_size added
 *
 * Revision 1.19  2008-10-12 21:05:12  tino
 * Error handling and tino_alloc_alignedO
 *
 * Revision 1.18  2008-09-01 20:18:13  tino
 * GPL fixed
 *
 * Revision 1.17  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.16  2008-05-04 04:00:35  tino
 * Naming convention for alloc.h
 *
 * Revision 1.13  2007/08/06 02:36:08  tino
 * TINO_REALLOC0 and TINO_REALLOC0_INC
 *
 * Revision 1.10  2007/03/25 22:53:33  tino
 * free()s added with convenience wrappers
 *
 * Revision 1.9  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.8  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.7  2004/05/20 20:45:45  tino
 * tino_realloc0ob added
 *
 * Revision 1.6  2004/04/29 22:36:54  tino
 * forgot some prefixes
 *
 * Revision 1.5  2004/04/20 23:51:38  tino
 * Hashing added (untested!)
 *
 * Revision 1.4  2004/04/13 10:51:54  tino
 * Starts to work like it seems
 *
 * Revision 1.3  2004/03/26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 */

#ifndef tino_INC_alloc_h
#define tino_INC_alloc_h

#include "err.h"
#include "fatal.h"
#include "debug.h"

/** Free without sideeffect
 */
static void
tino_freeO(void *p)
{
  int	e;

  if (!p)
    return;
  e	= errno;
  free(p);
  errno	= e;
}

/** Free a const pointer
 */
static void
tino_free_constO(const void *p)
{
  return tino_freeO((void *)p);
}

/* This is portable
 */
#define	TINO_FREE_NULL(X)	do { void *tmp=(X); (X)=0; free(tmp); } while (0)

/** Convenience: Free and NULL pointer
 *
 * THIS IS NOT PORTABLE (pointers might not match)
 *
 * Change tino_free_null(&x) into TINO_FREE_NULL(x)!
 */
static void
tino_free_nullUb(void **p)	/* not portable	*/
{
  void	*old;

  if (!p)
    return;
  old	= *p;
  *p	= 0;
  tino_freeO(old);
}


/** Check if RET is a suitable return value.
 * If none, then return NULL and free buffer.
 */
static char *
tino_free_return_bufN(char *ret, char *buf)
{
  if (ret==buf)
    return ret;
  tino_freeO(buf);
  return ret;
}

/** Realloc when downsizing, this is, if the request is not honored,
 * return the old buffer.
 */
static void *
tino_realloc_downsizeO(void *ptr, size_t len)
{
  void	*tmp;

  xDP(("(%p,%ld)", ptr, (long)len));
  tmp	= realloc(ptr, len);
  xDP(("() %p", tmp));
  return tmp ? tmp : ptr;
}

static void *
tino_reallocN(void *ptr, size_t len)
{
  void	*tmp;

  xDP(("(%p,%ld)", ptr, (long)len));
  tmp	= ptr ? realloc(ptr, len) : malloc(len);
  xDP(("() %p", tmp));
  return tmp;
}

static void *
tino_mallocN(size_t len)
{
  void	*tmp;

  xDP(("(%ld)", (long)len));
  tmp	= malloc(len);
  xDP(("() %p", tmp));
  return tmp;
}

static void
tino_OOM(size_t len)
{
  tino_err("FTLM100E %llu alloc failed", (unsigned long long)len);
}

static void *
tino_reallocO(void *ptr, size_t len)
{
  void	*tmp;

  xDP(("(%p,%ld)", ptr, (long)len));
  tmp	= tino_reallocN(ptr, len);
  if (!tmp)
    tino_OOM(len);
  xDP(("() %p", tmp));
  return tmp;
}

#define	TINO_REALLOC0(ptr,count,increment)	((ptr)=tino_realloc0O((ptr), (count)*sizeof *(ptr), (increment)*sizeof *(ptr)))
#define TINO_REALLOC0_INC(ptr,count,increment)	(TINO_REALLOC0(ptr,count,increment), (count)+=(increment), (ptr))
static void *
tino_realloc0O(void *buf, size_t len, size_t increment)
{
  void	*ptr;

  ptr	= tino_reallocO(buf, len+increment);
  memset((char *)ptr+len, 0, increment);
  return ptr;
}

static void *
tino_realloc0obO(void *buf, size_t len, size_t increment, size_t element)
{
  return tino_realloc0O(buf, len*element, increment*element);
}

static void *
tino_allocO(size_t len)
{
  return tino_reallocO(NULL, len);
}

static void *
tino_alloc0O(size_t len)
{
  void	*tmp;

  tmp	= tino_allocO(len);
  memset(tmp, 0, len);
  return tmp;
}

#define TINO_ALLOC_ALIGN_PAGE	((size_t)4096)

/** Round *len to the next full pagesize (which must be a power of two).
 */
static int
tino_alloc_align_size_nO(size_t *len, int pagesize)
{
  int	delta;

  tino_FATAL(pagesize & (pagesize-1));
  delta	= *len & (pagesize-1);
  if (delta)
    *len	+= pagesize-delta;
  return delta;
}

static int
tino_alloc_align_sizeO(size_t *len)
{
  return tino_alloc_align_size_nO(len, TINO_ALLOC_ALIGN_PAGE);
}

static void *
tino_alloc_alignedO(size_t len)
{
  void	*ptr;
  int	err;

  err   = posix_memalign(&ptr, TINO_ALLOC_ALIGN_PAGE, len);
  if (err)
    {
      errno	= err;
      tino_OOM(len);
      ptr	= 0;
    }
  return ptr;
}

static void *
tino_memdupO(const void *ptr, size_t len)
{
  void		*tmp;

  tino_FATAL(!ptr);
  tmp	= tino_allocO(len);
  memcpy(tmp, ptr, len);
  return tmp;
}

static char *
tino_strdupO(const char *s)
{
  char		*buf;

  tino_FATAL(!s);
  buf	= strdup(s);
  if (!buf)
    tino_OOM(strlen(buf)+1);
  return buf;
}

static char *
tino_strdupN(const char *s)
{
  char		*buf;

  if (!s)
    return 0;
  buf	= strdup(s);
  if (!buf)
    tino_OOM(strlen(buf)+1);
  return buf;
}

static const char *
tino_strsetO(const char **ptr, const char *s)
{
  tino_FATAL(!ptr);
  if (*ptr)
    free((char *)*ptr);
  return *ptr	= tino_strdupO(s);
}

#endif
