/* Sparse integer indexed data array.
 *
 * The array works with signed 64 bit keys.
 * The low order ENTRY_BITS bits are mapped to the entries
 * which are kept in the NODE.  The remaining higher order
 * bits define the NODE number.
 *
 * Each NODE stores it's number.  The parent NODE always
 * has a lower number than all of it's subnodes.  Hence NODEs
 * with higher numbers are mapped into subtrees below the
 * current node.
 *
 * The mapping to subnodes is based on levels.  The top node
 * always is on level 64, while the level of a subnode is the
 * number of the highest 1 bit of the node's number.  (Node 0
 * always becomes the top node, hence is on level 64.)
 *
 * In the level calculation of subnodes, the parent's level
 * and above are ignored in the level calculation.
 * Hence each node has exactly one less than it's level
 * of possible subnotes (as higher levels are mapped
 * somewhere else by the parent of the active node).
 *
 * That should be quite efficient:
 *
 * Access is O(log M) where M=2^64, so constant access time
 * regardless on how many integers are mapped.
 *
 * Similarily constant time for insertion and deletion.
 *
 * Bigger holes are skipped, so it uses less memory.
 *
 * Consecutive integers stay near each other.
 *
 * Minimum is immediately available.
 *
 * Can be extended to support next, prev and max in future.
 *
 * Can be extended to support insertion order with an
 * additional sparse mapping table.
 */

#include "mem.h"

#define	ARRAY_KEY_BITS		64
#define	CONS3(A,B,C)		A ## B ## C
#define	ARRAY_TYPE(BASE,BITS)	CONS3(BASE, BITS, _t)
#define ARRAY_KEY_TYPE		ARRAY_TYPE(int, ARRAY_KEY_BITS)
#define ARRAY_NODE_TYPE		ARRAY_TYPE(uint,ARRAY_KEY_BITS)
#define	ARRAY_ENTRY_BITS	8
#define	ARRAY_ENTRY_COUNT	(1<<ARRAY_ENTRY_BITS)
#define	ARRAY_ENTRY_MASK	(ARRAY_ENTRY_COUNT-1)

typedef ARRAY_KEY_TYPE ArrayKeyT;
typedef ARRAY_NODE_TYPE ArrayNodeT;

typedef struct ARRAY ARRAY;
struct ARRAY
  {
    struct array_node	*top, *free;
  };

typedef struct array_node	*ArrayNode;
struct array_node
  {
    void *		ent[ARRAY_ENTRY_COUNT];	/* 4..256	*/
    ArrayNode		*sub;			/* ->0..64	*/
    ArrayNode		*parent, next;
    ArrayNodeT		nr;			/* node number	*/
    int			level;	/* top==64.  is size of ->sub	*/
    int			ent_min, ent_max;	/* used ent	*/
  };

/* 64-__builtin_clzll(nr) isn't portable and does not work with 0	*/
static int
NodeLevel(ArrayNodeT nr, int max)
{
  int	lv;

  if (max>0)
    nr	&= (1ull<<max)-1ull;	/* works for max=0..64	*/
  lv	= 64;
  if (!(nr & 0xffffffff00000000ull)) { lv -= 32; nr <<= 32; }
  if (!(nr & 0xffff000000000000ull)) { lv -= 16; nr <<= 16; }
  if (!(nr & 0xff00000000000000ull)) { lv -=  8; nr <<=  8; }
  if (!(nr & 0xf000000000000000ull)) { lv -=  4; nr <<=  4; }
  if (!(nr & 0xc000000000000000ull)) { lv -=  2; nr <<=  2; }
  if (!(nr & 0x8000000000000000ull)) { lv -=  1; nr <<=  1; }
  return lv;
}

static ArrayNode
ArrayNodeNew(ARRAY arr, int nr, int level)
{
  ArrayNode	node;

  node		= alloc0(sizeof *node);
  node->nr	= 0;
  node->level	= level;
  node->ent_min	= -1;
  node->ent_max	= -1;
}

static ArrayNode
ArrayNodeLookup(ARRAY arr, ArrayNode *parent, int lookup, ArrayNodeT nr, int level)
{
  ArrayNode	node;

  FATAL(!parent || level<0 || level>ARRAY_KEY_BITS);
  if ((node = *parent)==0)
    return lookup ? 0 : *parent = ArrayNodeNew(arr, nr, level);

  FATAL(level != node->level);
  if (node->nr == nr)
    return node;

  /* if the current node is free, allocate it
   */

  /* Allocate subnodes, as we need it	*/
  if (!node->sub)
    node->sub	= ArrayNodeSub(node);

  /* look into subnodes	*/
  if (node->nr < nr)
    {
      int msb;

      msb	= NodeLevel(nr, level);
      FATAL(msb >= level);
      return ArrayNodeLookup(arr, &node->sub[msb], nr, msb);
    }

  /* node->nr > nr */
  000;
}


static ArrayNode *
ArrayNodeGet(ARRAY *arr, int lookup, ArrayNodeT nr)
{
  return ArrayNodeLookup(arr, &arr->top, lookup, nr, ARRAY_KEY_BITS-ARRAY_ENTRY_BITS);
}

static void **
ArrayEntryPtr(ARRAY *arr, ArrayKeyT key)
{
  struct ArrayNode	*node;
  int			nr;

  nr	= (int)key & ((1<<ARRAY_ENTRY_BITS)-1);
  node	= ArrayNodeGet(arr, 0, ((ArrayNodeT)key)>>ARRAY_ENTRY_BITS);
  if (node->min>nr)
    node->ent_min	= nr;
  if (node->ent_max < nr)
    node->ent_max	= nr;
  return &node->entry[nr];
}

static void **
ArrayEntryLookup(ARRAY *arr, ArrayKeyT key)
{
  struct ArrayNode	*node;
  int			nr;

  nr	= (int)key & ((1<<ARRAY_ENTRY_BITS)-1);
  node	= ArrayNodeGet(arr, 1, ((ArrayNodeT)key)>>ARRAY_ENTRY_BITS);
  return node ? &node->entry[nr] : 0;
}

static void *
arrayGet(ARRAY *arr, ArrayKeyT key, size_t len)
{
  void	**ptr;

  ptr	= ArrayEntryPtr(arr, key);
  if (! *ptr)
    *ptr = alloc0(len);
  return *ptr;
}

static void *
arraySet(ARRAY *arr, ArrayKeyT key, const void *data, size_t len)
{
  void	**ptr;
  int	copy;

  ptr	= ArrayEntryPtr(arr, key);
  if (*ptr == data)
    data	= 0;
  *ptr	= re_alloc(*ptr, len);
  if (data)
    memcpy(ptr, data, len);
  return *ptr;
}

static void
arrayFree(ARRAY *arr, ArrayKeyT key)
{
  void	**ptr, *tmp;

  ptr	= ArrayEntryLookup(arr, key);
  if (!ptr || !*ptr)
    return;

  tmp	= ptr;
  *ptr	= 0;
  free(*ptr);

  /* be lazy and do not free nodes here	*/
}

