/* $Header$
 *
 * THIS IS NOT (yet) THREAD SAFE!
 *
 * Auxilliary (allocated and initialized) buffers.  These mainly are
 * for use in printf()/scanf(), where you quickly need some assembly
 * buffers and don't want to think about free().
 *
 * You can create 32768 (0 to 32767) auxbufs.  This will never change,
 * not even on 128 bit platforms.  If you ever happen to need more,
 * this is not for you.
 *
 * This has an overhead of max 32768*(sizeof (size_t)+sizeof (void *))
 * (on 32 bit this is 256KB and on 64 bit 512KB).
 *
 * Copyright (C)2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.2  2007-08-06 15:43:45  tino
 * See ChangeLog
 *
 * Revision 1.1  2007/05/08 16:08:25  tino
 * See ChangeLog
 *
 */

#ifndef tino_INC_auxbuf_h
#define tino_INC_auxbuf_h

#include "alloc.h"

#define	TINO_AUXBUF		tino_auxbuf_sld20934msd9f23	/* if you use it, I change it, promised	*/
#define	TINO_AUXBUF_SPECIAL	-32768

/* U can't touch this
 *
 * THIS IS NOT (yet) THREAD SAFE!
 */
static struct TINO_AUXBUF
  {
    int		count;
    int		last;
    int		*len;
    void	**buf;	/* array of (real) void * (portable!)	*/
  } TINO_AUXBUF;

/** Get the last auxbuf nummer used
 */
static short
tino_auxbuf_lastOn(void)
{
  return TINO_AUXBUF.last;
}

static short
tino_auxbuf_last_setOn(short n)
{
  short	o;

  o			= TINO_AUXBUF.last;
  TINO_AUXBUF.last	= n&0x7fff;
  return o;
}

/** Get the maximum auxbuf nummer used
 *
 * This is, all buffers between 0..tino_auxbuf_max() are allocated.
 * Note that this can return -1 if no buffer has been used yet.
 */
static short
tino_auxbuf_maxOn(void)
{
  return TINO_AUXBUF.count-1;
}

static short
tino_auxbuf_get_nrOn(short n)
{
  if (n==TINO_AUXBUF_SPECIAL)
    n	= TINO_AUXBUF.last;
  if (n<0 || n>=TINO_AUXBUF.count)
    return -1;
  return n;
}

static short
tino_auxbuf_get_nr_filledOn(short n)
{
  n	= tino_auxbuf_get_nrO(n);
  if (n>=0 && TINO_AUXBUF.buf[n])
    return n;
  return -1;
}

/* NUL an auxbuf for safety
 */
static void
tino_auxbuf_clearOn(short n)
{
  if ((n=tino_auxbuf_get_nr_filledOn(n))>=0)
    memset(TINO_AUXBUF.buf[n], 0, TINO_AUXBUF.len[n] ? TINO_AUXBUF.len[n] : 1);
}

static void
tino_auxbuf_freeOn(short n)
{
  if ((n=tino_auxbuf_get_nr_filledOn(n))>=0)
    TINO_FREE_NULL(TINO_AUXBUF.buf[n]);
}

/** Create an auxilliary buffer of a given size.  You can create
 * 0..32767 (this is hardcoded and will never change!) auxbufs.
 *
 * THIS IS NOT (yet) THREAD SAFE!
 *
 * If N>=0 this accesses the given buffer of given size.
 *
 * If N<0 then the next -Nth higher auxbuf is used.  This primarily is
 * effective in printf type scenarios.
 *
 * Special: If n==TINO_AUXBUF_SPECIAL then the last auxbuf is accessed.
 */
static void *
tino_auxbufOn(short n /* -32768 to 32767 */, size_t len)
{
  if (n<0)
    n	= TINO_AUXBUF.last - n;
  n	&= 0x7fff;
  TINO_AUXBUF.last	= n;

  if (TINO_AUXBUF.count<=n)
    {
      TINO_AUXBUF.buf	= tino_realloc0ob(TINO_AUXBUF.buf, sizeof *TINO_AUXBUF.buf, TINO_AUXBUF.count, (int)n+1-TINO_AUXBUF.count);
      TINO_AUXBUF.count	= n+1;
    }

  if (!len)
    len	= 1;
  return (TINO_AUXBUF.buf[n] = tino_realloc(TINO_AUXBUF.buf[n], len));
}

static const char *
tino_auxbuf_sOn(short n, const char *s)
{
  char		*tmp;
  size_t	len;

  len	= strlen(s);
  tmp	= tino_auxbufOn(n, len+1);
  strncpy(tmp, s, len);
  tmp[len]	= 0;
  return tmp;
}

#undef	TINO_AUXBUF
#endif
