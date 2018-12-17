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

/* build sorted list on e->tmp
 * return the first unsorted element, NULL when none
 */
#define	SORTED_next	<
#define	SORTED_prev	>=
#define	SORTED(DIR)									\
        static TINO_ASSOC_EL								\
        tino_assoc_sorted_##DIR##_(struct _tino_assoc_sort *ord, TINO_ASSOC_EL e, TINO_ASSOC_EL x)	\
        {										\
          TINO_ASSOC_EL	n;								\
          for (; LIKELY(n=e->DIR); e = e->tmp = n)					\
            if (_tino_assoc_sort_cmp(ord, e, n) SORTED_##DIR 0)				\
              return (e->tmp = 0, n);							\
          return e->tmp = 0;								\
        }

SORTED(next)
SORTED(prev)

#undef SORTED
#undef SORTED_prev
#undef SORTED_next

/* Split TINO_ASSOC a beginning at element e1 up to (including) e2
 * into TINO_ASSOC b and TINO_ASSOC c
 */
static void
tino_assoc_sort_split(TINO_ASSOC a, TINO_ASSOC b, TINO_ASSOC c, TINO_ASSOC_EL e1, TINO_ASSOC_EL e2)
{
  *b		= *a;
  b->first	= 0;
  b->last	= 0;
  b->count	= 0;
  *c		= *b;

  if (e2)	/* the =???= is not empty	*/
    {
      TINO_ASSOC_EL	e;

      do
        {
          e	= e1->next;
          _tino_assoc_remove(a, e1);
          _tino_assoc_insert(b, e1, NULL);
          if (e1==e2)
            break;
          e1	= e->next;
          _tino_assoc_remove(a, e);
          _tino_assoc_insert(c, e, NULL);
        } while (e!=e2);
    }
}

static void
_tino_assoc_sort(TINO_ASSOC a, struct _tino_assoc_sort *ord)
{
  if (UNLIKELY(!a->first || !a->first->next))
    return;	/* less than 2 elements	*/

  /* find first non sorted element in e1	*/
  TINO_ASSOC_EL	e1;

  e1	= tino_assoc_sorted_next_(ord, a->first, a->last);
  if (UNLIKELY(!e1))
    return;	/* already sorted	*/

  /* optimization: try in reverse, perhaps we are just swapping:
   *
   * Stop the backward search at the current unsorted element.
   * Now create a list of sorted elements from the back of the array.
   * Restore the stop.
   */
  TINO_ASSOC_EL	e2, was;

  was		= e1->prev;
  e1->prev	= 0;
  e2		= tino_assoc_sorted_prev_(ord, a->last, e1);
  e1->prev	= was;
  /* If e2==NULL we hit the stop	*/

  /* Situation: first ==> e1 =???= e2 <== last
   *
   * Split the ??? into 2 unsorted lists: l2 and l3
   */
  struct tino_assoc	b, c;

  tino_assoc_sort_split(a, &b, &c, e1, e2);
  tino_assoc_sort(&b, order);
  tino_assoc_sort(&c, order);

  /* Merge tmp-lists:
   * a.first	following tmp
   * b.first	following next
   * c.first	following next
   * a.last	following tmp
   */
}

/* You must not access a list while it is sorted.
 *
 * Sort associative list.  This is stable if compare function is stable.
 */
static void
tino_assoc_sort(TINO_ASSOC a, int order)
{
  if (UNLIKELY(!a))
    return;	/* NULL list	*/

  struct _tino_assoc_sort	ord;

  ord.a		= a;
  ord.val	= order&3;
  ord.key	= ord.val != TINO_ASSOC_SORT_VAL;
  ord.revval	= order & TINO_ASSOC_SORT_REV;
  ord.revkey	= ord.val == TINO_ASSOC_SORT_VALKEYR ? !ord.revval : ord.revval;

  _tino_assoc_sort(a, &ord);
}

#undef cDP
#endif
