/* * NOT READY *
 *
 * No, this is not exactly a bitfield.
 * It's a memory of 'known' numbers,
 * where a known number means a 1 bit.
 *
 * It's thought for sparsely filled
 * (and later on huge) tables,
 * not meant to be efficient in handling bitfields.
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
  return -1;
}

#endif
