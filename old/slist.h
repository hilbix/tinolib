/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-04-29 22:35:44  tino
 * some typoos fixed
 *
 * Revision 1.1  2004/04/28 23:21:17  tino
 * Entered from md5backup / should be same as in Scylla+Charybdis
 */

#ifndef tino_INC_slist_h
#define tino_INC_slist_h

typedef struct tino_glistent	*TINO_GLIST_ENT;
typedef struct tino_glist	*TINO_GLIST;

typedef struct tino_glist	*TINO_SLIST;

struct tino_glistent
  {
    TINO_GLIST_ENT	next;
    void		*data;
  };
struct tino_glist
  {
    TINO_GLIST_ENT	list, *last;
    int			size;
    int			count;
  };

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
  ent->data	= 0;
  *list->last	= ent;
  list->last	= &ent->next;
  if (list->size)
    ent->data	= tino_alloc(list->size);
  list->count++;
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

#endif
