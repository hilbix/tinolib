/* $Header$
 *
 * C++ has following advantages:
 * - nicer syntax
 * - standard template library
 * - C routines are compatible to C++ programs.
 * C++ has following disadvantages:
 * - C++ routines are not compatible to C programs.
 * - You don't always have a C++ compiler / OO source.
 * - There is nothing in C++ you cannot do in C, too.
 * - You loose control of object creation too easily.
 * - No manual extensible object primitive (introspection).
 *
 * If in C++ primitive types were objects this would be
 * a point for me to switch permanently.
 * But this way I don't see any point why not stick to C.
 *
 * So this library is in plain C.
 * The object type is of low overhead.
 * And you have some very basic introspection.
 *
 * $Log$
 * Revision 1.1  2004-08-22 23:21:01  tino
 * sync to cvs
 *
 */

#define	TINO_OB_STR	"tino_str"
#define	TINO_OB_LIST	"tino_list"

typedef struct tino_ob tino_ob;

TINO_IMP

struct tino_ob
  {
    // Object name
    const char	*type;

    // Object properties
    size_t	size;
    unsigned	clear:1, weedout:1;

    // Object funktions
    void	(*construct)(void *);
    void	(*destruct)(void *);
    void	(*clone)(void *);

    // Object statistics
    long	count, allocs, frees;
  };

static tino_ob	*tino_ob_list;
static int	tino_ob_list_max;

/* fabric:
 * create an object of type "ob"
 */
TINO_PUB void *
tino_ob_new(const tino_ob *o)
{
  tino_ob	**p;

  if (!o)
    return 0;
  tino_ob_check(o);
  p	= tino_ob_alloc(o);
  p	= tino_alloc(o->size);
  if (o->clear)
    memset(p, 0, o->size);
  *p	= o;
  o->allocs++;
  o->count++;
  if (o->construct)
    o->construct(p);
  return p;
}

/* scrap:
 * free an object again
 */
TINO_PUB void
tino_ob_free(void *p)
{
  tino_ob	*o;

  if (!p)
    return;
  o	= *(tino_ob **)p;
  if (o)
    {
      tino_ob_check(o);
      o->frees++;
      o->count--;
      if (o->destruct)
	o->destruct(o);
      if (o->weedout)
	memset(p, 0, o->size);
    }
  free(p);
}

/* replicator:
 * clone an object
 */
TINO_PUB void *
tino_ob_clone(void *p)
{
  tino_ob	*o;

  if (!p)
    return;
  o	= *(tino_ob **)p;
  tino_ob_check(o);
  o->close(
}
