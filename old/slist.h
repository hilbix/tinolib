/* $Header$
 *
 * DEPRECATED (but still convenient)
 *
 * String and generic data lists
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
 * Revision 1.16  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.15  2007-09-18 20:16:50  tino
 * minor
 *
 * Revision 1.14  2007/01/28 03:02:07  tino
 * See ChangeLog 2006-12-16
 *
 * Revision 1.13  2006/11/15 03:33:53  tino
 * More NULL improvements (fatals or ignores)
 *
 * Revision 1.12  2006/11/15 03:25:35  tino
 * list==NULL now is ignored for most routines
 *
 * Revision 1.11  2006/08/22 23:57:03  tino
 * more general iteration added
 *
 * Revision 1.10  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.9  2005/03/04 00:43:04  tino
 * added *_destroy functions to free lists
 *
 * Revision 1.8  2004/11/23 22:28:15  tino
 * minor
 *
 * Revision 1.7  2004/10/05 02:04:55  tino
 * tino_glist_count added
 *
 * Revision 1.6  2004/05/21 02:36:47  tino
 * fatal include was missing
 *
 * Revision 1.5  2004/05/19 20:10:16  tino
 * glist_add_n added
 *
 * Revision 1.4  2004/05/19 05:00:04  tino
 * idea added
 *
 * Revision 1.3  2004/05/01 01:35:37  tino
 * new function: slist_iterate
 *
 * Revision 1.2  2004/04/29 22:35:44  tino
 * some typoos fixed
 *
 * Revision 1.1  2004/04/28 23:21:17  tino
 * Entered from md5backup / should be same as in Scylla+Charybdis
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
    free(ent->data);
  ent->next	= 0;
  ent->len	= 0;
  ent->data	= 0;
  free(ent);
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

static void
tino_glist_destroy(TINO_GLIST list)
{
  TINO_GLIST_ENT	e;

  while ((e=tino_glist_get(list))!=0)
    tino_glist_free(e);
  if (list)
    free(list);
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
tino_slist_destroy(TINO_SLIST list)
{
  tino_glist_destroy(list);
}

/* Only use the char * to augment the copy!
 */
static char *
tino_slist_add(TINO_SLIST list, const char *s)
{
  TINO_GLIST_ENT	ent;

  ent		= tino_glist_add((TINO_GLIST)list);
  ent->data	= tino_strdupO(s);
  return ent->data;
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
    free((char *)s);
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
