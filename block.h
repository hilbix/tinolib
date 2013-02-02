/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * NOT YET THREAD SAFE!
 *
 * Chained data blocks (preallocated, blockwise growing)
 *
 * The idea is, to be able to handle blocked data efficiently, with 0
 * copy if possible.  This was especially designed to do
 * en/decapsulations.
 *
 * Blocks can be chained.  Some operations work on the whole chain.
 * The library tries to keep chains in-order, this is to place chained
 * blocks behind each other.  However chains may be broken into
 * several pieces.
 *
 * Chaining operations are not very efficiently if it comes to random
 * access.
 *
 * Blocks can be immutable, this is for sharing.  The data of
 * immutable blocks cannot be altered.
 *
 * Usage:
 *	TINO_BLOCKS blocks;
 *	blocks	= tino_block_init(BUFSIZ, min_count);
 *	err	= tino_blocks_free(blocks);
 *
 *	name	= tino_blocks_flag(flag);
 *	val	= tino_blocks_get(flag);
 *	oldval	= tino_blocks_set(flag, value);
 *
 * The whole data chain can be saved and loaded, too:
 *	err	= tino_blocks_save(fd, format=0);
 *	blocks	= tino_blocks_load(fd);
 *
 *	TINO_BLOCK block;
 *
 *	block	= tino_block_new(blocks);
 *	nr	= tino_block_nr(block);
 *	block	= tino_blocks_find(nr);
 *	ok	= tino_block_verify(block, chain=1);
 *
 *	block	= tino_block_new_len(blocks, size);
 *	block	= tino_block_new_n(blocks, count);
 *	block	= tino_block_next(block);
 *	block	= tino_block_free(block);
 *	block	= tino_block_read(fd, max, &err);
 *	err	= tino_block_write(fd, block, max=-1);
 *	ptr	= tino_block_get(block, offset=0);
 *	offset	= tino_block_offset(block);		offset..len
 *	len	= tino_block_len(block);
 *	locks	= tino_block_relax(block);		unuse ptr of get
 *	locks	= tino_block_lock(block);
 *	locks	= tino_block_unlock(block, force_all=0);
 *	locks	= tino_block_locks(block);
 *
 *	block	= tino_block_prepend(block, ptr, len);
 *	block	= tino_block_append(block, ptr, len);
 *	offset	= tino_block_advance(block, n);		advance at front
 *	len	= tino_block_delete(block, n);		delete at end
 *
 *	leases	= tino_block_lease(block, chain=1);	make immutable
 *	leases	= tino_block_release(block, chain=1);	frees if leases=0
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * $Log$
 * Revision 1.5  2008-09-01 20:18:13  tino
 * GPL fixed
 *
 * Revision 1.4  2008-05-19 09:13:59  tino
 * tino_alloc naming convention
 *
 * Revision 1.3  2007-01-25 04:39:15  tino
 * Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
 */

#ifndef tino_INC_block_h
#define tino_INC_block_h

#include "fatal.h"
#include "alloc.h"

#if 0
#include <unistd.h>

#include "codec.h"
#endif

#define	cDP	TINO_DP_block

enum tino_blocks_flags
  {
    TINO_BLOCKS_FLAG_SIZE,
    TINO_BLOCKS_FLAG_BUFFERS,
    TINO_BLOCKS_FLAG_USED,
    TINO_BLOCKS_FLAG_MAX
  };

/* this way I cannot forget one accidentially	*/
static char *
tino_blocks_flag(enum tino_blocks_flags n)
{
  switch (n)
    {
    case TINO_BLOCKS_FLAG_USED:		return "Used buffers";
    case TINO_BLOCKS_FLAG_BUFFERS:	return "Allocated buffers";
    case TINO_BLOCKS_FLAG_SIZE:		return "Size of a buffer block";
    case TINO_BLOCKS_FLAG_MAX:		break;
    }
  return 0;
}

struct tino_blocks_chain
  {
    struct tino_blocks_chain	*next;
    long			first_blocknr;
  };

typedef struct tino_blocks
  {
    struct tino_blocks_chain	*chain;
    long			flags[TINO_BLOCKS_FLAG_MAX];
  } *TINO_BLOCKS;

struct tino_block_chain
  {
  };

typedef struct tino_block
  {
    size_t	fill;	/* Usual acutal fill position	*/
    size_t	max;	/* Maximum alocated data size	*/
    size_t	off;	/* Offset from the begining which is free	*/
    char	*data;
  };

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* internal functions */

static TINO_BLOCKS
tino_blocks_init(int size, int count)
{
  TINO_BLOCKS	*b;

  if (size<=0)
    size	= BUFSIZ;
  if (size<64)
    size	= 64;
  b	= tino_alloc0O(sizeof *b);
  000;
}

/* Verify the blocks and free if nothing more in use
 */
static int
tino_blocks_free(TINO_BLOCKS blocks)
{
  tino_FATAL("tino_blocks_free() not yet implemented");
  return 1;
}

static void
tino_blocks_save(int fd)
{
  000;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* main functions */



/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* add functions */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* get functions */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* IO functions */


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Specials */


#undef	cDP
#endif
