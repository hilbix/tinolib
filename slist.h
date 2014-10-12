/* DEPRECATED (but still convenient)
 *
 * String and generic data lists
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

#ifndef tino_INC_slist_h
#define tino_INC_slist_h

#include "fatal.h"
#include "alloc.h"

typedef struct tino_glistent	*TINO_GLIST_ENT;
typedef struct tino_glist	*TINO_GLIST;

typedef struct tino_glist	*TINO_SLIST;

struct tino_glistent
  {
    TINO_GLIST_ENT	next;
    size_t		len;
    void		*data;
  };
struct tino_glist
  {
    TINO_GLIST_ENT	list, *last;
    int			size;
    int			count;
  };

static __inline__ int
tino_glist_count(TINO_GLIST list)
{
  if (!list)
    return 0;
  return list->count;
}

static TINO_GLIST
tino_glist_new(size_t size)
{
  TINO_GLIST	list;

  list		= tino_allocO(sizeof *list);
  list->list	= 0;
  list->last	= &list->list;
  list->size	= size;
  list->count	= 0;
  return list;
}

static TINO_GLIST_ENT
tino_glist_add(TINO_GLIST list)
{
  TINO_GLIST_ENT	ent;

  tino_FATAL(!list);
  ent		= tino_allocO(sizeof *ent);
  ent->next	= 0;
  ent->len	= list->size;
  ent->data	= ent->len ? tino_allocO(ent->len) : 0;
  *list->last	= ent;
  list->last	= &ent->next;
  list->count++;
  return ent;
}

static TINO_GLIST_ENT
tino_glist_add_ptr(TINO_GLIST list, void *ptr)
{
  TINO_GLIST_ENT	ent;

  ent		= tino_glist_add(list);
  tino_FATAL(ent->data);
  ent->data	= ptr;
  return ent;
}

static TINO_GLIST_ENT
tino_glist_add_n(TINO_GLIST list, const void *ptr, size_t len)
{
  TINO_GLIST_ENT	ent;

  ent		= tino_glist_add(list);
  tino_FATAL(ent->data);
  ent->len	= len;
  ent->data	= ptr ? tino_memdupO(ptr, len) : tino_allocO(len);
  return ent;
}

static void *
tino_glist_add_data(TINO_GLIST list)
{
  return tino_glist_add(list)->data;
}

static TINO_GLIST_ENT
tino_glist_get(TINO_GLIST list)
{
  TINO_GLIST_ENT	ent;

  xDP(("tino_glist_get(%p)", list));
  if (!list || (ent=list->list)==0)
    return 0;
  list->count--;
  if ((list->list=ent->next)==0)
    list->last	= &list->list;
  return ent;
}

static void
tino_glist_free(TINO_GLIST_ENT ent)
{
  xDP(("tino_glist_free(%p)", ent));
  if (!ent)
    return;
  if (ent->data)
    tino_freeO(ent->data);
  ent->next	= 0;
  ent->len	= 0;
  ent->data	= 0;
  tino_freeO(ent);
}

static void *
tino_glist_fetchfree(TINO_GLIST_ENT ent)
{
  void	*data;

  tino_FATAL(!ent);
  data		= ent->data;
  ent->data	= 0;
  tino_glist_free(ent);
  return data;
}

static TINO_GLIST
tino_glist_clear(TINO_GLIST list)
{
  TINO_GLIST_ENT	e;

  while ((e=tino_glist_get(list))!=0)
    tino_glist_free(e);
  return list;
}

static void
tino_glist_destroy(TINO_GLIST list)
{
  if (list)
    tino_freeO(tino_glist_clear(list));
}

static TINO_GLIST_ENT
tino_glist_first(TINO_GLIST list)
{
  if (!list)
    return 0;
  return list->list;
}

static TINO_GLIST_ENT
tino_glist_next(TINO_GLIST_ENT ent)
{
  tino_FATAL(!ent);
  return ent->next;
}

static void *
tino_glist_data(TINO_GLIST_ENT ent)
{
  tino_FATAL(!ent);
  return ent->data;
}

static void *
tino_glist_step(TINO_GLIST list, TINO_GLIST_ENT *ent)
{
  void			*p;
  TINO_GLIST_ENT	e;

  if ((e= *ent)==0)
    e	= tino_glist_first(list);
  p	= 0;
  if (e)
    {
      p	= tino_glist_data(e);
      e	= tino_glist_next(e);
    }
  *ent	= e;
  return p;
}


/**********************************************************************/

static TINO_SLIST
tino_slist_new(void)
{
  return (TINO_SLIST)tino_glist_new((size_t)0);
}

static void
tino_slist_clear(TINO_SLIST list)
{
  tino_glist_clear(list);
}

static void
tino_slist_destroy(TINO_SLIST list)
{
  tino_glist_destroy(list);
}

/* Only use the char * to augment the copy!
 */
static char *
tino_slist_add(TINO_SLIST list, const char *s)
{
  return tino_glist_add_ptr((TINO_GLIST)list, tino_strdupO(s))->data;
}

static TINO_SLIST
tino_slist_init(int argc, const char * const *argv)
{
  int	i;
  TINO_SLIST	list;

  list	= tino_slist_new();
  for (i=0; i<argc; i++)
    tino_slist_add(list, argv[i]);
  return list;
}

static const char *
tino_slist_get(TINO_SLIST list)
{
  TINO_GLIST_ENT	ent;

  xDP(("tino_slist_get(%p)", list));
  if ((ent=tino_glist_get((TINO_GLIST)list))==0)
    return 0;
  return tino_glist_fetchfree(ent);
}

static void
tino_slist_free(const char *s)
{
  if (s)
    tino_free_constO(s);
}

/* Iterate over an slist
 * Easy to implement, but not outside, as there iteration shall be unknown
 */
static void
tino_slist_iterate(TINO_SLIST list, void (*fn)(const char *, void *), void *u)
{
  TINO_GLIST		g=list;
  TINO_GLIST_ENT	e;

  if (g)
    for (e=g->list; e; e=e->next)
      fn(e->data, u);
}

static void
tino_slist_iterate_c(TINO_SLIST list, void (*fn)(const char *, const void *), const void *u)
{
  tino_slist_iterate(list, (void (*)(const char *, void *))fn, (void *)u);
}

static int
tino_slist_iterate_0(TINO_SLIST list, int (*fn)(const char *, void *), void *u)
{
  TINO_GLIST		g=list;
  TINO_GLIST_ENT	e;
  int			ret;

  if (g)
    for (e=g->list; e; e=e->next)
      if ((ret=fn(e->data, u))!=0)
        return ret;
  return 0;
}

static int
tino_slist_iterate_0_c(TINO_SLIST list, int (*fn)(const char *, const void *), const void *u)
{
  return tino_slist_iterate_0(list, (int (*)(const char *, void *))fn, (void *)u);
}

#endif
