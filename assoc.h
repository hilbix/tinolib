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
    struct tino_assoc_el	*e, *p;
    const void			*k;
    void			*v;
    int				r:1, c:1;
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

TINO_INLINE(static void
_tino_assoc_remove(TINO_ASSOC a, TINO_ASSOC_EL e))
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
  e->next	= 0;
  e->prev	= 0;
}

/* If an element is given, prepend to the given element
 * (insert it at the given position).
 *
 * NULL means, append at the end, a->first means prepend to assoc.
 */
TINO_INLINE(static void
_tino_assoc_insert(TINO_ASSOC a, TINO_ASSOC_EL e, TINO_ASSOC_EL p))
{
  _tino_assoc_remove(a, e);

  if (a->first == p)
    a->first	= e;
  if (p)
    {
      e->next	= p;
      e->prev	= p->prev;
      if (e->prev)
        e->prev->next	= e;
      p->prev	= e;
    }
  else
    {
      e->prev	= a->last;
      if (a->last)
        a->last->next	= e;
      a->last	= e;
    }
}

static TINO_ASSOC_EL
_tino_assoc_new(TINO_ASSOC a, const void *key)
{
  TINO_ASSOC_EL	e;

  e		= tino_allocO(sizeof *e);
  e->next	= 0;
  e->prev	= 0;
  e->k		= a->ops->knew(a->user, key);
  e->v		= 0;
  e->_const	= 0;

  _tino_assoc_insert(a, e, NULL);
  a->count++;
  return e;
}

static void
_tino_assoc_vset(TINO_ASSOC a, TINO_ASSOC_EL e, void *v, int _const)
{
  if (e->v)
    a->ops->vfree(a->user, e->v);
  e->v		= a->ops->vnew(a->user, v);
  e->_const	= !!_const;
}

static void
_tino_assoc_free(TINO_ASSOC a, TINO_ASSOC_EL e)
{
  _tino_assoc_remove(a, e);
  a->count--;

  a->ops->vfree(a->user, e->v);
  e->v		= 0;

  a->ops->kfree(a->user, (void *)e->k);
  e->k		= 0;

  tino_freeO(e);
}

/**********************************************************************/

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
    _tino_assoc_free(a, a->first);
  TINO_FATAL_IF(a->count);
  if (a->_alloc)
    tino_freeO(a);
}

/* Set key to const value
 */
static void
tino_assoc_set(TINO_ASSOC a, const void *k, const void *v)
{
  cDP(("(%p,%p,%p)", a, k, v));
  _tino_assoc_vset(a, _tino_assoc_bykey(a, k, 1), (void *)v, 1);
}

/* Set key to nonconst value
 */
static void
tino_assoc_nset(TINO_ASSOC a, const void *k, void *v)
{
  cDP(("(%p,%p,%p)", a, k, v));
  _tino_assoc_vset(a, _tino_assoc_bykey(a, k, 1), v, 0);
}

/* Get pointer to const value
 */
static const void *
tino_assoc_get(TINO_ASSOC a, const void *k)
{
  TINO_ASSOC_EL	e;

  cDP(("(%p,%p)", a, k));
  e	= _tino_assoc_bykey(a, k, 0);
  cDP(("() %p %p", e, e ? e->v : NULL));
  return e ? e->v : NULL;
}

/* Get pointer to nonconst value
 */
static void *
tino_assoc_nget(TINO_ASSOC a, const void *k)
{
  TINO_ASSOC_EL	e;

  cDP(("(%p,%p)", a, k));
  e	= _tino_assoc_bykey(a, k, 0);
  cDP(("() %p %p", e, e ? e->v : NULL));
  TINO_FATAL_COND(e->_const, "nonconstant access to constant assoc element");
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
  _tino_assoc_free(a, e);
  cDP(("() 0 (ok)"));
  return 0;
}

/* Access to key, value and nonconstant value	*/
static const void *tino_assoc_key(TINO_ASSOC_ITER i) { return i->k; }
static const void *tino_assoc_val(TINO_ASSOC_ITER i) { return i->v; }
static       void *tino_assoc_nval(TINO_ASSOC_ITER i) { TINO_FATAL_COND(i->c, "nonconstant access to constant assoc element"); return i->v; }

/* Important for the movement routines:
 *
 * You must call tino_assoc_more() afterwards to make the movement to your current position!
 */

/* Move to the first element.  Returns:
 * true:	there is a first element
 * false:	empty assoc, tino_assoc_more() will free the iterator
 */
TINO_INLINE(static int
tino_assoc_first(TINO_ASSOC_ITER i))
{
  i->p	= i->r ? i->a->last : i->a->first;
  return i->p != 0;
}

/* Move to the last element.  Returns:
 * true:	there is a last element
 * false:	empty assoc, tino_assoc_more() will free the iterator
 */
TINO_INLINE(static int
tino_assoc_last(TINO_ASSOC_ITER i))
{
  i->p	= i->r ? i->a->first : i->a->last;
  return i->p != 0;
}

/* Move to the next element.
 * returns true:	there is more
 * false:		we are at the end, tino_assoc_more() will free the iterator
 */
TINO_INLINE(static int
tino_assoc_next(TINO_ASSOC_ITER i))
{
  cDP(("(%a) %p", i, i->p));
  if (UNLIKELY(!i->p))
    return 0;
  i->p	= i->r ? i->p->prev : i->p->next;
  return 1;
}

/* Move to the previous element (opposite direction).
 * returns true:	moved to previous element
 * false:		already at the first element, unknown if tino_assoc_more() will free the iterator
 *
 * If this returns false, you are either on the first element or have an empty list.
 * In this case, use tino_assoc_first() to see if tino_assoc_more() will free the iterator.
 *
 * if (tino_assoc_prev(i))
 *   {
 *     tino_assoc_more(i);
 *     // work with element
 *   }
 * else if (!tino_assoc_first(i))
 *   {
 *     break;	// empty list, avoid i to be freed by not calling tino_assoc_more()
 *   }
 * else
 *   {
 *     tino_assoc_more(i);
 *     // we are now working with the first element
 *   }
 */
TINO_INLINE(static int
tino_assoc_prev(TINO_ASSOC_ITER i))
{
  struct tino_assoc_el	*p;

  cDP(("(%a) %p", i, i->p));
  if (UNLIKELY(!i->p))
    p	= i->r ? i->a->last : i->a->first;
  else
    p	= i->r ? i->p->next : i->p->prev;
  if (!p)
    return 0;
  i->p	= p;
  return 1;
}

/*
 * for (i=tino_assoc_iter(a, 0); tino_assoc_more(i); tino_assoc_next(i))
 *   {
 *     // deletion of the current key is safe, as long you do not change the future elements
 *     tino_assoc_key(i);	// access key
 *     tino_assoc_val(i);	// access value
 *     tino_assoc_deli(i);	// delete the current entry
 *   }
 * // iter is automatically freed by tino_assoc_more
 * or:
 * i	= tino_assoc_iter(a, 0);
 * while (tino_assoc_step(i))
 *   {
 *     // see above
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
  i->e	= 0;
  i->k	= 0;
  i->v	= 0;
  i->c	= 0;
  i->r	= !!rev;
  tino_assoc_first(i);
  cDP(("() %p", i));
  return i;
}

static void
tino_assoc_end(TINO_ASSOC_ITER i)
{
  cDP(("(%a)", i));
  i->a	= 0;
  i->e	= 0;
  i->p	= 0;
  i->k	= 0;
  i->v	= 0;
  tino_freeO(i);
}

/* Do the movement and find out if there is a current element
 * true:	there is more
 * false:	at the end, free the iterator
 *
 * If you do not want to free the iterator, do not call this routine.
 * You can use the return values of the movement routines as indicator.
 */
TINO_INLINE(static int
tino_assoc_more(TINO_ASSOC_ITER i))
{
  cDP(("(%a) %p", i, i->p));
  if (UNLIKELY(!i->p))
    {
      tino_assoc_end(i);
      return 0;
    }
  i->e	= i->p;
  i->c	= i->p->_const;
  i->k	= i->p->k;
  i->v	= i->p->v;
  return 1;
}

/* Convenience: tino_assoc_next and tino_assoc_more combined
 * returns true:	there is on element
 * false:		we are at the end, iterator is freed
 */
TINO_INLINE(static int
tino_assoc_step(TINO_ASSOC_ITER i))
{
  if (UNLIKELY(!tino_assoc_more(i)))
    return 0;
  tino_assoc_next(i);
  return 1;
}

/* Convenience: remove the value at current iterator position
 *
 * No bad siedeffect if called twice or more often.
 */
TINO_INLINE(void
tino_assoc_deli(TINO_ASSOC_ITER i))
{
  if (UNLIKELY(!i->e))
    return;
  i->k	= 0;
  i->v	= 0;
  _tino_assoc_free(i->a, i->e);
  i->e	= 0;
}

/* Convenience: Change current iterator position to constant value
 */
TINO_INLINE(void
tino_assoc_seti(TINO_ASSOC_ITER i, const void *v))
{
  if (UNLIKELY(!i->e))
    return;
  _tino_assoc_vset(i->a, i->e, (void *)v, 1);
}

/* Convenience: Change current iterator position to nonconstant value
 */
TINO_INLINE(void
tino_assoc_nseti(TINO_ASSOC_ITER i, void *v))
{
  if (UNLIKELY(!i->e))
    return;
  _tino_assoc_vset(i->a, i->e, v, 0);
}

#undef	cDP
#endif

