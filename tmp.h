/* Temporary Buffers
 *
 * Usage:
 * static TINO_TMP tmp;
 *
 * buf	= tino_tmp_set(&tmp, size);	// define
 * ..
 * ptr	= tino_tmp_inc(&tmp);		// increment reference count
 * ..
 * tino_tmp_dec(&tmp);			// decrement reference count
 *
 * tino_tmp_new(&tmp);			// register
 * ..
 * tino_tmp_free(&tmp);			// unregister
 *
 * Difference to allocated buffers:
 *
 * - Use without new is ok.  Then the buffer ignores reference count until de-registered.
 * - Multiple new is ok, it does nothing or re-register after de-registered.
 * - Multiple free is ok, this only de-registers the tmp.
 * - Use after free is ok, as long as reference count is positive.
 * - Mixing up things usually is ok, as you are not working with pointers.
 */

#ifndef tino_INC_tmp_h
#define tino_INC_tmp_h

#include "alloc.h"
#include "debug.h"

#define	cDP	TINO_DP_tmp

#define	TMP(X)	tino_tmp_##X##__r9sd23s49k0k

typedef struct { long tmp; } TINO_TMP;	/* yes, it's not a pointer	*/

struct TMP(tmp)
  {
    void		*data;
    unsigned long	ref;
    size_t		len;
    unsigned long	free;
  };

static struct
  {
    unsigned long	max;
    struct TMP(tmp)	*tmp;
    long		free;
  } TMP(tmp);

static void
TMP(expand)(void)
{
  struct TMP(tmp)	*ptr;
  unsigned long		old;

  old		=  TMP(tmp).max;

  TMP(tmp).max	= old + 10 + old/100;
  TMP(tmp).tmp	= tino_reallocO(TMP(tmp).tmp, TMP(tmp).max * sizeof *TMP(tmp).tmp);
  memset(TMP(tmp).tmp+old, 0, (TMP(tmp).max-old) * sizeof *TMP(tmp).tmp);

  if (!old)	/* bucket 0 always is unused	*/
    old++;
  for (ptr = &TMP(tmp).tmp[old]; old < TMP(tmp).max; old++)
    {
      ptr->free		= TMP(tmp).free;
      TMP(tmp).free	= old;
    }
}

static long
TMP(add)(void)
{
  long	n;

  if (!TMP(tmp).free)
    TMP(expand)();

  n			= TMP(tmp).free;
  TMP(tmp).free		= TMP(tmp).tmp[n].free;
  TMP(tmp).tmp[n].free	= 0;
  return n;
}

static struct TMP(tmp) *
TMP(get0)(TINO_TMP *tmp)
{
  TINO_FATAL_IF(!tmp || tmp->tmp<0);
  TINO_FATAL_IF(tmp->tmp >= TMP(tmp).max);
  return &TMP(tmp).tmp[tmp->tmp];
}

static struct TMP(tmp) *
TMP(get)(TINO_TMP *tmp)
{
  TINO_FATAL_IF(!tmp);
  if (!tmp->tmp)
    tmp->tmp	= TMP(add)();
  return TMP(get0)(tmp);
}

static void
TMP(free)(TINO_TMP *tmp)
{
  struct TMP(tmp)	*buf;
  long			n;


  n	= tmp->tmp;
  buf	= TMP(get0)(tmp);
  TINO_FATAL_IF(n != tmp->tmp);

  if (!buf->free)
    return;
  if (buf->ref>0)
    return;

  TINO_FATAL_IF(buf == TMP(tmp).tmp);
  TINO_FATAL_IF(buf->ref);
  TINO_FATAL_IF(buf->free>0);

  tino_freeO(buf->data);

  buf->data	= 0;
  buf->len	= 0;
  buf->free	= TMP(tmp).free;

  TMP(tmp).free	= n;
  tmp->tmp	= 0;    
}

static void *
TMP(set)(struct TMP(tmp) *buf, size_t len)
{
  if (buf->len != len || !buf->data)
    {
      buf->data	= tino_reallocO(buf->data, len ? len : 1);
      buf->len	= len;
    }
  return buf->data;
}

static void *
tino_tmp_set(TINO_TMP *tmp, size_t len)
{
  return TMP(set)(TMP(get)(tmp), len);;
}

static size_t
tino_tmp_len(TINO_TMP *tmp)
{
  return TMP(get0)(tmp)->len;
}

static void *
tino_tmp_inc(TINO_TMP *tmp)
{
  struct TMP(tmp)	*buf;

  buf	= TMP(get0)(tmp);
  buf->ref++;
  TINO_FATAL_IF(!buf->data);
  return buf->data;
}

static void
tino_tmp_dec(TINO_TMP *tmp)
{
  struct TMP(tmp)	*buf;

  buf	= TMP(get0)(tmp);
  buf->ref--;
  TMP(free)(tmp);
}

static void
tino_tmp_new(TINO_TMP *tmp)
{
  struct TMP(tmp)	*buf;

  buf	= TMP(get)(tmp);
  TINO_FATAL_IF(buf->free>0);
  buf->free	= -1;
  if (!buf->data)
    TMP(set)(buf, 0);
}

static void
tino_tmp_free(TINO_TMP *tmp)
{
  TMP(free)(tmp);
}

#undef	TMP
#undef	cDP
#endif

