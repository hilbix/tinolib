/* $Header$
 *
 * $Log$
 * Revision 1.7  2004-10-05 02:04:55  tino
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
  return list->count;
}

static TINO_GLIST
tino_glist_new(size_t size)
{
  TINO_GLIST	list;

  list		= tino_alloc(sizeof *list);
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

  ent		= tino_alloc(sizeof *ent);
  ent->next	= 0;
  ent->len	= list->size;
  ent->data	= ent->len ? tino_alloc(ent->len) : 0;
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
  ent->data	= ptr ? tino_memdup(ptr, len) : tino_alloc(len);
  return ent;
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

  data		= ent->data;
  ent->data	= 0;
  tino_glist_free(ent);
  return data;
}

static TINO_SLIST
tino_slist_new(void)
{
  return (TINO_SLIST)tino_glist_new((size_t)0);
}

static void
tino_slist_add(TINO_SLIST list, const char *s)
{
  TINO_GLIST_ENT	ent;

  ent		= tino_glist_add((TINO_GLIST)list);
  ent->data	= tino_strdup(s);
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

  for (e=g->list; e; e=e->next)
    fn(e->data, u);
}

#endif
