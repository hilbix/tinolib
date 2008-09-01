/* $Header$
 *
 * Generic simple sortable array
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.4  2008-09-01 20:18:13  tino
 * GPL fixed
 *
 * Revision 1.3  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.2  2007-08-06 15:43:45  tino
 * See ChangeLog
 *
 * Revision 1.1  2007/01/28 03:02:07  tino
 * See ChangeLog 2006-12-16
 */

#ifndef tino_INC_array_h
#define tino_INC_array_h

#include "fatal.h"
#include "alloc.h"
#include "debug.h"

#define	cDP	TINO_DP_array

typedef struct tino_array	*TINO_ARRAY;

struct tino_array
  {
    struct tino_array_ops	*ops;
    int				nr, fill;
    void			*elements;
  };

struct tino_array_ops
  {
    int			entsize;
    const char		*entname;
    const char		*(*to_string)(const void *);
    int			(*sort_cmp)(const void *, const void *);
    void		(*copy_in)(void *, const void *);
    void		(*copy_out)(void *, const void *);
    void		(*free)(void *);
  };

/**********************************************************************/

/** This is a bug
 *
 * void ** does not point to a generic pointer.
 */
static void
tino_array_string_ptr_copy(void *ent, const void *a)
{
  cDP(("(%p,%p) '%s'", ent, a, *(const void * const *)a));
  *(const void **)ent	= *(const void * const *)a;
}

static const char *
tino_array_string_ptr_to_string(const void *a)
{
  return *(const char * const *)a;
}

static int
tino_array_string_cmp(const void *a, const void *b)
{
  return strcmp(*(const char * const *)a, *(const char * const *)b);
}

static struct tino_array_ops tino_array_string_ptr =
  { sizeof (const char *)
    , "const char *"
    , tino_array_string_ptr_to_string
    , tino_array_string_cmp
    , tino_array_string_ptr_copy
    , tino_array_string_ptr_copy
    , NULL
  };

/**********************************************************************/

static TINO_ARRAY
tino_array_init(TINO_ARRAY a, struct tino_array_ops *ops)
{
  a->ops	= ops;
  a->nr		= 0;
  a->fill	= 0;
  a->elements	= 0;
  return a;
}

static TINO_ARRAY
tino_array_new(struct tino_array_ops *ops)
{
  return tino_array_init(tino_allocO(sizeof (struct tino_array)), ops);
}

static void
tino_array_free(TINO_ARRAY a)
{
  char	*p;
  int	i;

  cDP(("(%p)", a));
  a->fill	= 0;
  if (a->ops->free)
    for (p=a->elements, i=a->nr; --i>=0; p+=a->ops->entsize)
      a->ops->free(p);
  if (a->elements)
    free(a->elements);
  a->nr		= 0;
  a->elements	= 0;
  free(a);
}

/* Add an entry to the end of the array.
 *
 * Needs a pointer to the entry.  The entry must not be freed!
 */
static void
tino_array_add(TINO_ARRAY a, void *ent)
{
  cDP(("(%p,%p)", a, ent));
  if (a->nr >= a->fill)
    {
      a->fill		+= 1024+(a->fill>>8);
      a->elements	= tino_reallocO(a->elements, a->ops->entsize*a->fill);
    }
  a->ops->copy_in(((char *)a->elements)+(a->nr*a->ops->entsize), ent);
  a->nr++;
}

static void
tino_array_add_p(TINO_ARRAY a, const void *ent)
{
  tino_array_add(a, &ent);
}

/* Access an array entry
 */
static int
tino_array_get(TINO_ARRAY a, int n, void *ent)
{
  if (n<0 || n>=a->nr)
    return 0;
  a->ops->copy_out(ent, ((char *)a->elements)+(n*a->ops->entsize));
  return 1;
}

static void
tino_array_sort(TINO_ARRAY a)
{
  qsort(a->elements, a->nr, a->ops->entsize, a->ops->sort_cmp);
}

static int
tino_array_count(TINO_ARRAY a)
{
  return a->nr;
}

#undef cDP
#endif
