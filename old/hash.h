/* $Header$
 *
 * Hash maps.
 *
 * Why re-invent the wheel (as hash maps are part of libstd)?
 * Well, I like to use algorithms I know and I can alter as I need it.
 * Like perfect hashing, or boyer moore search.
 * And I like it to be as simple as it can be.
 * Especially the memory management.
 * Usually all the other things out there do not fit in my control paranoia.
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.4  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.3  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.2  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/04/20 23:51:38  tino
 * Hashing added (untested!)
 */

#ifndef tino_INC_hash_h
#define tino_INC_hash_h

#include "debug.h"
#include "fatal.h"
#include "alloc.h"

typedef struct tino_hash_map		tino_hash_map;
typedef union  tino_hash_map_val	tino_hash_map_val;
typedef struct tino_hash_map_raw	tino_hash_map_raw;
typedef struct tino_hash_map_handle	tino_hash_map_handle;
struct tino_hash_map_nodes
  {
    int				nodes;
    struct tino_hash_map_node	*node;
  };
struct tino_hash_map_raw
  {
    size_t			len;
    void			*ptr;
  };
union tino_hash_map_val
  {
    char			c;
    int				i;
    unsigned			u;
    long			l;
    unsigned long		ul;
    long long			ll;
    unsigned long long		ull;
    void			*ptr;
    tino_hash_map_raw		raw;
    struct tino_hash_map_nodes	hash;
  };
struct tino_hash_map_node
  {
    tino_hash_map_val		val;
    tino_hash_map_raw		key;
  };
struct tino_hash_map
  {
    int				fn;
    int				len;
    struct tino_hash_map_nodes	hash;
  };

static void
tino_hash_imp_init(struct tino_hash_map_nodes *h, int len)
{
  if (len<7)
    len		= 7;
  h->node	= tino_alloc0O((len+1)*sizeof *h->node);
  h->nodes	= len;
}

static struct tino_hash_map_node *
tino_hash_imp_store_key(struct tino_hash_map_node *node, const void *ptr, size_t len)
{
  node->key.ptr	= tino_memdupO(ptr, len);
  node->key.len	= len;
  return node;
}

#include "hasher.h"

/* This rehashing is no ordinary rehashing.
 *
 * We just move the old data into the NULL bucket.
 * This way we can ensure that even when we have a double-hash
 * we will always only extend by one new sub-hash.
 *
 * Such that the have a very fast insertion.
 * Yes, there should be some special algorithm to re-distribute
 * HASH maps in case we become extremely disorganized
 * (such like the hash becoming a linked list).
 *
 * However leave optimizations to future.
 */
static void
tino_hash_imp_rehash(struct tino_hash_map_nodes *h, struct tino_hash_map_node *node, int fn)
{
  struct tino_hash_map_node	old;

  old		= *node;
  node->key.ptr	= 0;
  node->key.len	= fn+1;
  tino_hash_imp_init(&node->val.hash, h->nodes-1);
  node->val.hash.node[node->val.hash.nodes]	= old;
}

static struct tino_hash_map_node *
tino_hash_imp_parent(tino_hash_map *map, const void *ptr, size_t len, int create)
{
  struct tino_hash_map_nodes	*h;
  struct tino_hash_map_node	*node;
  int				fn;

  if (!map->hash.nodes)
    {
      if (!create)
	return 0;
      tino_hash_imp_init(&map->hash, map->len);
    }
  if (!ptr || !len)
    {
      /* NULL case
       * Hand out the NULL bucket.
       */
      node	= &map->hash.node[map->hash.nodes];
      if (node->key.len)
	return node;
      if (!create)
	return 0;
      node->key.len	= 1;	/* It's used now	*/
      return node;
    }
  tino_FATAL(!ptr || !len);
  fn	= map->fn;
  h	= &map->hash;
  for (;;)
    {
      int	hash;

      hash	= hasher(ptr, len, fn)%h->nodes;
      node	= &h->node[hash];
      if (!node->key.len)
	{
	  /* Empty bucket
	   */
	  if (!create)
	    return 0;
	  tino_hash_imp_store_key(node, ptr, len);
	  return node;
	}
      if (node->key.ptr)
	{
	  if (node->key.len==len && !memcmp(ptr, node->key.ptr, len))
	    return node;	/* Match	*/
          if (!create)
	    return 0;
	  /* Well, we have a double hit, but we want to add another entry.
	   * Create a new sub-hash to store the values in.
	   */
	  tino_hash_imp_rehash(h, node, fn);
	  h	= &node->val.hash;
	  continue;
	}
      fn	= node->key.len;	/* Oh yes, well .. dirty reuse	*/
      h		= &node->val.hash;
      /* Test the NULL bucket of the sub-list.
       * In case we are extremely unlucky
       * we run through a linked list this way.
       * However its sure that we always terminate.
       */
      node	= &h->node[h->nodes];
      if (node->key.len==len && !memcmp(ptr, node->key.ptr, len))
	return node;		/* Match	*/
    }
}

static void
tino_hash_map_init(tino_hash_map *map, int len, int fn)
{
  map->fn	= fn;
  map->len	= len;
  memset(&map->hash, 0, sizeof map->hash);
}

#if 0
static int
tino_hash_del_ptr(tino_hash_map *map, const void *s, size_t len)
{
  struct tino_hash_map_node	*node;

  node	= tino_hash_imp_parent(map, s, len, 0);
  if (!node)
    return 0;
  tino_hash_imp_free(node);
  return 1;
}
#endif

tino_hash_map_val *
tino_hash_get_ptr(tino_hash_map *map, const void *s, size_t len)
{
  struct tino_hash_map_node	*node;

  node	= tino_hash_imp_parent(map, s, len, 0);
  if (!node)
    return 0;
  return &node->val;
}

tino_hash_map_val *
tino_hash_add_ptr(tino_hash_map *map, const void *s, size_t len)
{
  struct tino_hash_map_node	*node;

  node	= tino_hash_imp_parent(map, s, len, 1);
  tino_FATAL(!node);
  return &node->val;
}
#endif
