/* NOT THREAD SAFE *
 *
 * Generic associative array (k,v) with access via k
 *
 * This is a very trivial implementation, no optimizations so far!
 * Most operations are O(n), you have been warned.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_assoc_h
#define tino_INC_assoc_h

#include "fatal.h"
#include "alloc.h"
#include "debug.h"

#define	cDP	TINO_DP_assoc

typedef struct tino_assoc		*TINO_ASSOC;
typedef const struct tino_assoc_ops	*TINO_ASSOC_OPS;
typedef struct tino_assoc_el		*TINO_ASSOC_EL;
typedef struct tino_assoc_iter		*TINO_ASSOC_ITER;

struct tino_assoc
  {
    TINO_ASSOC_OPS		ops;
    struct tino_assoc_el	*first, *last;
    void			*user;
    int				count;
    int				_alloc:1;
  };

struct tino_assoc_iter
  {
    struct tino_assoc		*a;
    const struct tino_assoc_el	*p;
    const void			*k, *v;
    int				r;
  };

struct tino_assoc_el
  {
    struct tino_assoc_el	*prev, *next;
    const void			*k;
    void			*v;
    int				_const:1;
#ifdef	TINO_ASSOC_MIXIN
    struct tino_assoc_el	*tmp;
#define	TINO_HAS_ASSOC_MIXIN
#endif
  };

TINO_INLINE(static int tino_assoc_count(TINO_ASSOC a)) { return a->count; }

struct tino_assoc_ops
  {
#if 0
    int			ksize, vsize;					/* 0=dynamic	*/
#endif
    const char		*name;						/* just for debugging	*/
    int			(*kcmp)(void *, const void *, const void *);	/* compare keys	*/
    int			(*vcmp)(void *, const void *, const void *);	/* compare values	*/
    void *		(*knew)(void *, const void *);			/* create key	*/
    void *		(*vnew)(void *, const void *);			/* create value	*/
    void		(*kfree)(void *, void *);			/* free key	*/
    void		(*vfree)(void *, void *);			/* free value	*/
#if 0
    const char		*(*to_string)(const void *);
    int			(*sort_cmp)(const void *, const void *);
    void		(*copy_in)(void *, const void *);
    void		(*copy_out)(void *, const void *);
    void		(*free)(void *);
#endif
  };

/**********************************************************************/

/* Standard String to String Relation
 */

static int
tino_assoc_str2str_cmp(void *u, const void *a, const void *b)
{
  return a==b ? 0 : !a ? -1 : !b ? 1 : strcmp(a, b);
}

static void *
tino_assoc_str2str_dup(void *u, const void *p)
{
  return tino_strdupN(p);
}

static void
tino_assoc_str2str_free(void *u, void *p)
{
  tino_freeO(p);
}

static struct tino_assoc_ops tino_assoc_str2str =
  { /* sizeof (const char *)
  , sizeof (void *)
  , */ "assoc(string to string)"
  , tino_assoc_str2str_cmp
  , tino_assoc_str2str_cmp
  , tino_assoc_str2str_dup
  , tino_assoc_str2str_dup
  , tino_assoc_str2str_free
  , tino_assoc_str2str_free
  };

/**********************************************************************/

static TINO_ASSOC_EL
_tino_assoc_new(TINO_ASSOC a, const void *key)
{
  TINO_ASSOC_EL	e;

  e		= tino_allocO(sizeof *e);
  e->next	= 0;
  e->prev	= a->last;
  e->k		= a->ops->knew(a->user, key);
  e->v		= a->ops->knew(a->user, key);
  e->_const	= 0;

  a->count++;

  if (a->last)
    a->last->next	= e;
  a->last	= e;
  if (!a->first)
    a->first	= e;

  return e;
}

/* lookup is O(n) for now
 *
 * This should be greatly optimized with perfect hashing.
 * But for this we have to implement a hashing infrastructure for keys.
 */
static TINO_ASSOC_EL
_tino_assoc_bykey(TINO_ASSOC a, const void *key, int create)
{
  TINO_ASSOC_EL	e;

  for (e=a->first; e; e=e->next)
    if (!a->ops->kcmp(a->user, key, e->k))
      return e;
  return create ? _tino_assoc_new(a, key) : 0;
}

static void
_tino_assoc_vset(TINO_ASSOC a, TINO_ASSOC_EL e, void *v, int _const)
{
  if (e->v)
    a->ops->vfree(a->user, e->v);
  e->v		= a->ops->vnew(a->user, v);
  e->_const	= _const;
}

static void
_tino_assoc_remove(TINO_ASSOC a, TINO_ASSOC_EL e)
{
  TINO_FATAL_IF(!e);

  if (e->next)
    e->next->prev	= e->prev;
  if (e->prev)
    e->prev->next	= e->next;
  if (a->first == e)
    a->first	= e->next;
  if (a->last == e)
    a->last	= e->prev;
}

/**********************************************************************/

static TINO_ASSOC
tino_assoc_init(TINO_ASSOC a, TINO_ASSOC_OPS ops, void *user)
{
  cDP(("(%p,%p,%p)", a, ops, user));
  if (!a)
    {
      a		= tino_allocO(sizeof *a);
      a->_alloc	= 1;
    }
  a->ops	= ops ? ops : &tino_assoc_str2str;
  a->user	= user ? user : a;
  a->count	= 0;
  a->first	= 0;
  a->last	= 0;
  cDP(("() %p", a));
  return a;
}

TINO_INLINE(static TINO_ASSOC
tino_assoc_new(TINO_ASSOC_OPS ops))
{
  return tino_assoc_init(NULL, ops, NULL);
}

static void
tino_assoc_free(TINO_ASSOC a)
{
  cDP(("(%p)", a));
  while (a->first)
    _tino_assoc_remove(a, a->first);
  TINO_FATAL_IF(a->count);
  if (a->_alloc)
    tino_freeO(a);
}

static void
tino_assoc_set(TINO_ASSOC a, const void *k, void *v)
{
  cDP(("(%p,%p,%p)", a, k, v));
  return _tino_assoc_vset(a, _tino_assoc_bykey(a, k, 1), v, 0);
}

static void
tino_assoc_setc(TINO_ASSOC a, const void *k, const void *v)
{
  cDP(("(%p,%p,%p)", a, k, v));
  return _tino_assoc_vset(a, _tino_assoc_bykey(a, k, 1), (void *)v, 1);
}

static void *
tino_assoc_get(TINO_ASSOC a, const void *k)
{
  TINO_ASSOC_EL	e;

  cDP(("(%p,%p)", a, k));
  e	= _tino_assoc_bykey(a, k, 0);
  cDP(("() %p %p", e, e ? e->v : NULL));
  TINO_FATAL(("nonconstant access to constant assoc element", e->_const));
  return e ? e->v : NULL;
}

static const void *
tino_assoc_getc(TINO_ASSOC a, const void *k)
{
  TINO_ASSOC_EL	e;

  cDP(("(%p,%p)", a, k));
  e	= _tino_assoc_bykey(a, k, 0);
  cDP(("() %p %p", e, e ? e->v : NULL));
  return e ? e->v : NULL;
}

/* Remove assoc by key
 */
static int
tino_assoc_del(TINO_ASSOC a, const void *k)
{
  TINO_ASSOC_EL	e;

  cDP(("(%p,%p)", a, k));
  e	= _tino_assoc_bykey(a, k, 0);
  if (!e)
    {
      cDP(("() -1 (no)"));
      return -1;
    }
  _tino_assoc_remove(a, e);
  cDP(("() 0 (ok)"));
  return 0;
}

/*
 * for (i=tino_assoc_iter(a, 0); tino_assoc_more(i); tino_assoc_next(i))
 *   {
 *     // deletion of the current key is safe, as long you do not access the element afterwards
 *     tino_assoc_key(i);
 *     tino_assoc_val(i);
 *   }
 * // iter is automatically freed
 * or:
 * i	= tino_assoc_iter(a, 0);
 * while (tino_assoc_step(i))
 *   {
 *   }
 * // iter is automatically freed
 */
static TINO_ASSOC_ITER
tino_assoc_iter(TINO_ASSOC a, int rev)
{
  TINO_ASSOC_ITER	i;

  cDP(("(%a, %d)", a, rev));
  i	= tino_allocO(sizeof *i);
  i->a	= a;
  i->p	= rev ? a->last : a->first;
  i->k	= NULL;
  i->v	= NULL;
  i->r	= rev;
  cDP(("() %p", i));
  return i;
}

static const void *tino_assoc_key(TINO_ASSOC_ITER i) { return i->k; }
static const void *tino_assoc_val(TINO_ASSOC_ITER i) { return i->v; }

static void
tino_assoc_end(TINO_ASSOC_ITER i)
{
  cDP(("(%a)", i));
  tino_freeO(i);
}

TINO_INLINE(static int
tino_assoc_more(TINO_ASSOC_ITER i))
{
  cDP(("(%a) %p", i, i->p));
  if (UNLIKELY(!i->p))
    {
      tino_assoc_end(i);
      return 0;
    }
  i->k	= i->p->k;
  i->v	= i->p->v;
  return 1;
}

TINO_INLINE(static int
tino_assoc_next(TINO_ASSOC_ITER i))
{
  cDP(("(%a) %p", i, i->p));
  if (UNLIKELY(!i->p))
    return 0;
  i->p	= i->r ? i->p->prev : i->p->next;
  return 1;
}

TINO_INLINE(static int
tino_assoc_step(TINO_ASSOC_ITER i))
{
  if (UNLIKELY(!tino_assoc_more(i)))
    return 0;
  tino_assoc_next(i);
  return 1;
}

#undef	cDP
#endif
