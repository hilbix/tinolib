/* $Header$
 *
 * No, this is not exactly a bitfield.
 * It's a memory of 'known' numbers,
 * where a known number means a 1 bit.
 *
 * It's thought for sparsely filled
 * (and later on huge) tables,
 * not meant to be efficient in handling bitfields.
 *
 * $Log$
 * Revision 1.1  2004-04-07 02:23:11  tino
 * Empty version yet added
 *
 */

#ifndef tino_INC_bitfield_h
#define tino_INC_bitfield_h

typedef struct tino_bitfield
  {
    long long	min, max;
    long long	cnt;
  } TINO_BITFIELD;

static void
tino_bitfield_init(TINO_BITFIELD *f)
{
  f->min	= 0;
  f->max	= -1;
  f->cnt	= 0;
}

static void
tino_bitfield_free(TINO_BITFIELD *f)
{
  000;
}

static void
tino_bitfield_set(TINO_BITFIELD *f, long long bit)
{
  000;
}

static void
tino_bitfield_clear(TINO_BITFIELD *f, long long bit)
{
  000;
}

static int
tino_bitfield_get(TINO_BITFIELD *f, long long bit)
{
  000;
}

#endif
