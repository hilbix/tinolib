/* NOT THREAD SAFE *
 *
 * Stable MergeSort for associative list O(n log n)
 *
 * This is a very trivial implementation, no optimizations so far!
 * Most operations are O(n), you have been warned.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_assoc_sort_h
#define tino_INC_assoc_sort_h

#define	TINO_ASSOC_MIXIN
#include "assoc.h"
#ifndef	TINO_HAS_ASSOC_MIXIN
#error	"assoc-sort.h must be included before assoc.h"
#endif

#define	cDP	TINO_DP_assoc

/* sort order	*/
#define	TINO_ASSOC_SORT_KEY	0	/* sort by key				*/
#define	TINO_ASSOC_SORT_VAL	1	/* sort by val				*/
#define	TINO_ASSOC_SORT_VALKEY	2	/* sort by val then key			*/
#define	TINO_ASSOC_SORT_VALKEYR	3	/* sort by val then reversed key	*/
#define	TINO_ASSOC_SORT_REV	4	/* reverse sort order (ADD this)	*/

struct _tino_assoc_sort
  {
    TINO_ASSOC	a;
    int		key, val;
    int		revkey, revval;
  };

/* Append a list of following sorted entries in ->tmp
 * Return the first non-sorted element.
 */

TINO_INLINE(static int
_tino_assoc_sort_cmp(struct _tino_assoc_sort *ord, TINO_ASSOC_EL e1, TINO_ASSOC_EL e2))
{
  int	d;

  if (ord->val)
{
d = ord->a->ops->vcmp(ord->a, e1->v, e2->v);
      if (d)
        return ord->revval ? -d : d;
    }
  if (ord->key)
    {
      d = ord->a->ops->kcmp(ord->a, e1->k, e2->k);
      if (d)
        return ord->revkey ? -d : d;
    }
  return 0;
}

#define	SORTED_next	<
#define	SORTED_prev	>=
#define	SORTED(DIR)									\
	static TINO_ASSOC_EL								\
	_tino_assoc_sorted_##DIR(struct _tino_assoc_sort *ord, TINO_ASSOC_EL e)		\
	{										\
	  TINO_ASSOC_EL	n;								\
	  for (; LIKELY(n=e->DIR); e = e->tmp = n)					\
	    if (_tino_assoc_sort_cmp(ord, e, n) SORTED_##DIR 0)				\
	      return (e->tmp = 0, n);							\
	  return e->tmp = 0;								\
	}

SORTED(next)
SORTED(prev)

/* Merge the ->tmp lists of merge into main
 */
static void
_tino_assoc_sort_merge(struct _tino_assoc_sort *ord, TINO_ASSOC_EL main, TINO_ASSOC_EL merge)
{
  while (main->tmp)
    {
    }
}

static void
tino_assoc_sort(TINO_ASSOC a, int order)
{
  if (!a)
    return;

  if (!a->first || !a->first->next)
    return;	/* less than 2 elements	*/

  struct _tino_assoc_sort	ord;

  ord.a		= a;
  ord.val	= order&3;
  ord.key	= ord.val != TINO_ASSOC_SORT_VAL;
  ord.revval	= order & TINO_ASSOC_SORT_REV;
  ord.revkey	= ord.val == TINO_ASSOC_SORT_VALKEYR ? !ord.revval : ord.revval;

  TINO_ASSOC_EL	e1, e2, prev;

  e1	= _tino_assoc_sorted_next(&ord, a->first);
  if (!e1)
    return;	/* already sorted	*/

  /* optimization: try in reverse, perhaps we are just swapping	*/
  prev		= e1->prev;
  e1->prev	= 0;
  e2		= _tino_assoc_sorted_prev(&ord, a->last);
  e1->prev	= prev;

  /* first ==> e1 =??= e2 <== last	*/
  _tino_assoc_sort_merge(&ord, a->first, a->last, 1);
  if (!e2)
    return;	/* we are sorted, it was: first ==> e1 <== last	*/

  /* now it is: first ==> e1 =??= e2	*/
  TINO_FATAL_IF(e2 != a->last);

  do
    {
      /* merge in next sorted portion	*/
      e2	= _tino_assoc_sorted_next(&ord, e1);
      _tino_assoc_sort_merge(&ord, a->first, e1, 0);
    } while (LIKELY(e1=e2));
}

#undef cDP
#endif
