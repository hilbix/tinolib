/* A generic stable mergesort for "integer indexed something"
 * (This is NOT the right algorithm for linked lists nor sequential media.)
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 *
 * How does this work?
 *
 *	CPU	MEM	CALLS	<- worst
 * a)	N	N		create an index array (0..n-1)
 * b)	N log N	N+N/2+1	N log N	cmp over the index array
 * c)	2N	N	N	swaps according to the index array
 *
 * Overall:
 *
 * CPU	O(N log N)
 * RAM	O(N)
 */

#define	mergesort	index_mergesort		/* protect against MacOSX	*/

#ifndef	MERGESORT_USER_TYPE
#define	MERGESORT_USER_TYPE	void *
#endif

struct _mergesort
  {
    MERGESORT_USER_TYPE	user;
    int (		*cmp  )(MERGESORT_USER_TYPE, int, int);
    void *(		*alloc)(MERGESORT_USER_TYPE, size_t);
    void (		*free )(MERGESORT_USER_TYPE, void *);
  };

static void
_mergesort(struct _mergesort *_, int *a, int n)
{
  int	m, *l, *r;
  int	i, j, k;

  if (n<2)
    return;

  m	=  n/2;
  n	-= m;

  l	= _->alloc(_->user, m * sizeof *l);
  r	= _->alloc(_->user, n * sizeof *r);

  for (i=m; --i>=0; l[i] = a[i]);
  for (i=n; --i>=0; r[i] = a[m+i]);

  _mergesort(_, l, m);
  _mergesort(_, r, n);

  i=j=k=0;
  while (i<m && j<n)	a[k++] = _->cmp(_->user, l[i], r[j]) < 0 ? l[i++] : r[j++];
  while (i<m)		a[k++] = l[i++];
  while (j<n)		a[k++] = l[j++];

  _->free(_->user, l);
  _->free(_->user, r);
}

static void *
mergesort(MERGESORT_USER_TYPE user
        , int n
        , int (*cmp    )(MERGESORT_USER_TYPE, int, int)
        , void (*swap  )(MERGESORT_USER_TYPE, int, int)
        , void *(*alloc)(MERGESORT_USER_TYPE, size_t)
        , void (*free  )(MERGESORT_USER_TYPE, void *))
{
  struct _mergesort	m;
  int			*a, i;

  if (n<2)
    return user;

  m.user	= user;
  m.cmp		= cmp;
  m.alloc	= alloc;
  m.free	= free;

  a	= alloc(user, n * sizeof *a);
  for (i=n; --i>=0; a[i] = i);

  _mergesort(&m, a, n);

  /* a is the list of indexes into the original array.
   */
  for (i=0; i<n; )
    {
      int	j = a[i];
      if (i == j)
        {
          i++;
          continue;
        }
      swap(user, i, j);
      a[i]	= a[j];
      a[j]	= j;	/* one position now ok	*/
    }

  free(user, a);
  return user;
}

