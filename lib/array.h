/* Sparse integer indexed data array.
 *
 * ARRAY key are int64_t
 *
 * ARRAY a = { 0 };
 * copy= arraySet(a, key, ptr, sizeof *ptr);	// copies data into array, or resizes
 * ptr = arrayGet(a, key, sizeof *ptr);		// creates ptr of given size NULled on the fly
 * ptr = arrayHas(a, key);			// access ptr, NULL if not found
 * arrayDel(a, key);				// release memory, returns 0 if not found
 *
 * ARRAY is not meant to be efficient on small data, as it always allocates the buffer.
 * If you want better efficiency, use a MAP.
 * (This might be re-implemented using MAPs in future.)
 *
 * Background:
 *
 * The low order ENTRY_BITS are mapped to the entries
 * which are kept in a NODE.  The remaining higher order
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
 * That should be efficient enough on sparse and
 * nonsparse cases:
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

typedef struct array_node	*ArrayNode;

struct ARRAY
  {
    ArrayNode top;
  };
typedef struct ARRAY ARRAY[1];

struct array_node
  {
    void *		ent[ARRAY_ENTRY_COUNT];	/* 4..256	*/
    ArrayNode		*sub;			/* ->0..64	*/
#if 0
    ArrayNode		*parent, next;
#endif
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
  return node;
}

static ArrayNode *
ArrayNodeSub(ARRAY arr, ArrayNode node)
{
  FATAL(node->level <= 0 || node->level > ARRAY_KEY_BITS);
  return alloc0(node->level * sizeof(ArrayNode));
}

/* XXX TODO SMELL: Too long, to difficult to understand
 */
static ArrayNode
ArrayNodeLookup(ARRAY arr, ArrayNode *parent, int lookup, ArrayNodeT nr, int level)
{
  ArrayNode	node, add, *sub;
  int		sublvl;

  FATAL(!parent || level<0 || level>ARRAY_KEY_BITS);
  if ((node = *parent)==0)
    return lookup ? 0 : (*parent = ArrayNodeNew(arr, nr, level));

  FATAL(level != node->level);
  if (node->nr == nr)
    return node;

  /* if the current node is free, allocate it
   */

  /* Allocate subnodes, as we need it	*/
  if (!node->sub)
    node->sub	= ArrayNodeSub(arr, node);

  /* look into subnodes	*/
  if (node->nr < nr)
    {
      sublvl	= NodeLevel(nr, level);
      FATAL(sublvl < 0 || sublvl >= level);
      return ArrayNodeLookup(arr, &node->sub[sublvl], lookup, nr, sublvl);
    }

  /* node->nr > nr, new nr replaces this one */
  add		= ArrayNodeNew(arr, nr, level);
  *parent	= add;

  /* Now we must insert node into the slot
   * it is always the minimum, so becomes the top node of the subtree
   */
  for (sublvl=level; node; )
    {
      sub	= node->sub;
      if (!sub)
        sub	= ArrayNodeSub(arr, node);
      add->sub	= sub;

      add	= node;
      add->sub	= 0;

      sublvl	= NodeLevel(add->nr, sublvl);
      add->level= sublvl;

      node	= sub[sublvl];
      sub[sublvl]= add;
    }

  return *parent;
}

/* NodeNr(Key) -> ArrayNode
 * lookup==0: creates ArrayNode if missing.
 * lookup==1: returns NULL is missing.
 */
static ArrayNode
ArrayNodeGet(ARRAY arr, int lookup, ArrayNodeT nr)
{
  return ArrayNodeLookup(arr, &arr->top, lookup, nr, ARRAY_KEY_BITS-ARRAY_ENTRY_BITS);
}

/* key -> pointer to data bucket
 * creates bucket if missing
 * See also: ArrayEntryLookup
 */
static void **
ArrayEntryPtr(ARRAY arr, ArrayKeyT key)
{
  ArrayNode	node;
  int		nr;

  nr	= (int)key & ((1<<ARRAY_ENTRY_BITS)-1);
  node	= ArrayNodeGet(arr, 0, ((ArrayNodeT)key)>>ARRAY_ENTRY_BITS);
  if (node->ent_min > nr)
    node->ent_min	= nr;
  if (node->ent_max < nr)
    node->ent_max	= nr;
  return &node->ent[nr];
}

/* key -> pointer to data bucket
 * returns NULL if missing
 * See also: ArrayEntryPtr
 */
static void **
ArrayEntryLookup(ARRAY arr, ArrayKeyT key)
{
  ArrayNode	node;
  int		nr;

  nr	= (int)key & ((1<<ARRAY_ENTRY_BITS)-1);
  node	= ArrayNodeGet(arr, 1, ((ArrayNodeT)key)>>ARRAY_ENTRY_BITS);
  return node ? &node->ent[nr] : 0;
}

/**********************************************************************
 * Public Interface
 * (do not use functions above)
 **********************************************************************/

/* return buffer (len is ignored and not checked!) for known key.
 * return newly allocated buffer if len NUL bytes for new key.
 */
static void *
arrayGet(ARRAY arr, ArrayKeyT key, size_t len)
{
  void	**ptr;

  ptr	= ArrayEntryPtr(arr, key);
  if (! *ptr)
    *ptr = alloc0(len);
  return *ptr;
}

/* store data[len] into key's buffer
 * returns allocated/resized buffer
 *
 * iff data==NULL or data==existingbuffer
 * then buffer is just resized
 */
static void *
arraySet(ARRAY arr, ArrayKeyT key, const void *data, size_t len)
{
  void	**ptr;

  ptr	= ArrayEntryPtr(arr, key);
  if (*ptr == data)
    data	= 0;
  *ptr	= re_alloc(*ptr, len);
  if (data)
    memcpy(ptr, data, len);
  return *ptr;
}

/* return ptr to buffer for known key
 * return NULL for unknown key
 */
static void *
arrayHas(ARRAY arr, ArrayKeyT key)
{
  void **ptr;

  ptr	= ArrayEntryLookup(arr, key);
  return ptr ? *ptr : 0;
}

/* return 1 if key known and deleted
 * return 0 else
 */
static int
arrayDel(ARRAY arr, ArrayKeyT key)
{
  void	**ptr, *tmp;

  ptr	= ArrayEntryLookup(arr, key);
  if (!ptr || !*ptr)
    return 0;

  tmp	= ptr;
  *ptr	= 0;
  free(tmp);

  /* be lazy and do not free nodes here	*/
  return 1;
}

