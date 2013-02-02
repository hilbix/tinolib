/* $Header$
 *
 * NOT READY YET *
 * UNIT TEST FAILS *
 *
 * Generic wildcard matches
 *
 * This is a binary safe regexp parser which supports sorting.
 *
 * Rationale:
 *
 * There are two major wildcard libraries out there:
 *
 * The POSIX regexp parser which cannot match 0 bytes.
 * The PERL regexp parser which is unusable as it may
 * use backtracking.
 *
 * Also both parser libraries are very difficult to use
 * and cannot be used for sorting easily.
 * So, sorry, I had to create another one.
 *
 * This here shall be small, fast, efficient and easy to use.
 * There are a lot of convenience routines for "just do it".
 *
 * It's easy to tell if everything is OK:
 * -1 means error (compile error etc., except where noted)
 * 0 means success (true, except where noted)
 * 1 means "not true" (false, except where noted)
 *
 * // Either
 * TINO_WILD	w={0};	// no mem initialize
 * ...
 * tino_wild_free(&w);	// or tino_wild_reset(&w);
 * // or
 * TINO_WILD	w;
 * tino_wild_init(&w);	// memory init
 * ...
 * tino_wild_free(&w);	// or tino_wild_reset(&w);
 * // or
 * TINO_WILD	*w=tino_wild_init(NULL);
 * ...
 * tino_wild_free(w);	// do not use w afterwards!
 *
 * // in the following the first/second case is used:
 * // set global flags
 * tino_wild_set(&w, FLAG);
 * // compile in compares with local flags (0=success)
 * tino_wild_divert(&w, ref);
 * tino_wild_end(&w);
 * tino_wild_add_regex(&w, regex, FLAGS);
 * tino_wild_add_strcmp(&w, const char *, FLAGS);
 * tino_wild_add_memcmp(&w, const void *, size_t, FLAGS);
 * // do the compare
 * tino_wild_strcmp(&w, const char *);		// 0==match
 * tino_wild_memcmp(&w, const void *, size_t);	// 0==match
 * // more matches
 * tino_wild_match(&w);				// 0==another match
 * // match references (n=0:all, n==1..refs:parantheses)
 * tino_wild_refs(&w);				// number of refs
 * tino_wild_ref_offset(&w, n, &len);		// return offset
 * tino_wild_ref_len(&w, n, &offset);		// return len
 * // IMPORTANT: see sorting comment below!
 * tino_wild_sort_order(&w);
 * tino_wild_sort_strcmp(&w, const char *);	// return -2..2
 * tino_wild_sort_memcmp(&w, const void *, size_t);	// dito
 *
 * Copyright (C)2007-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.2  2008-09-01 20:18:15  tino
 * GPL fixed
 *
 * Revision 1.1  2007-08-06 02:57:40  tino
 * Added, it's just a stub
 */

#ifndef tino_INC_wild_h
#define tino_INC_wild_h

#define tino_wild_impl	tino_wild_sf0zsdvij293r8893fn2983f2p3t

struct tino_wild_impl
  {
  };
typedef struct tino_wild
  {
    struct tino_wild_impl	*impl;
    int				allocated;
    int				error;
    const void *		lastarg;	/* warning: this may point nowhere */
    size_t			arglen;
#define	tino_wild_cnt(W)	((W)->ref_count)
    int				ref_count;
#define	tino_wild_off(W,N)	((W)->ref_offsets[N])
    int				*ref_off;
#define tino_wild_len(W,N)	((W)->ref_len[N])
    int				*ref_len;
  } TINO_WILD;

static void
tino_wild_reset(TINO_WILD *w)
{
  int	a;

  if (!w)
    return;
  a		= w->alloated;
  if (w->impl)		/* safety	*/
    free(w->impl);
  w->impl	= 0;
  memset(w, 0, sizeof *w);
  w->allocated	= a;
}

/* Free memory.
 *
 * Free the structure only if it was allocated before
 */
static void
tino_wild_free(TINO_WILD *w)
{
  if (!w)
    return;
  tino_wild_reset(w);
  if (w->allocated)
    free(w);
}


/* Returns the references.  You can access them directly, too, see
 * defines above.  However this are convenience routines in case your
 * pointer is NULL.
 */
static int
tino_wild_refs(TINO_WILD *w)
{
  if (!w || w->error)
    return 0;
  return tino_wild_cnt(w);
}

static int
tino_wild_ref_offset(TINO_WILD *w, int n, size_t *len)
{
  if (!w || w->error)
    return 0;
  if (len)
    *len	= tino_wild_len(w,n);
  return tino_wild_off(w,n);
}

static int
tino_wild_ref_len(TINO_WILD *w, int n, size_t *off)
{
  if (!w || w->error)
    return 0;
  if (off)
    *off	= tino_wild_off(w,n);
  return tino_wild_len(w,n);
}


#define tino_wild_impl	$never_use_this$
#endif
