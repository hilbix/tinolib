/* $Header$
 *
 * helper routines
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
 * Revision 1.3  2005-12-05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.2  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/08/17 23:07:14  Administrator
 * added
 */

#ifndef tino_INC_helpers_h
#define tino_INC_helpers_h

#include "fatal.h"

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Physical low level IO	*/

/* Read something, ignore signals (EINTR)
 */
static int
tino_read_step(int fd, char *buf, size_t len)
{
  int	got;

  if (!len)
    return 0;

  while ((got=read(fd, buf, len))<0)
    if (errno!=EINTR && errno!=EAGAIN)
      return -1;
  tino_FATAL(got>len);
  return got;
}

/* Read everything, ignore signals (EINTR)
 * returns:
 * -1 on error
 * else bytes read
 * (note that this might be less then len)
 */
static int
tino_read(int fd, char *buf, size_t len)
{
  int	pos, got;

  for (pos=0; pos<len; )
    {
      got	= tino_read_step(fd, buf+pos, len-pos);
      if (got<=0)
	{
	  if (!got)
	    break;
	  return -1;
	}
      pos	+= got;
      tino_FATAL(pos>len);
    }
  return pos;
}

/* Write something, ignore signals (EINTR)
 * returns:
 * -1 on error
 * 0 on EOF or nothing to do
 * else bytes written
 */
static int
tino_write_step(int fd, const char *buf, size_t len)
{
  int	put;

  if (!len)
    return 0;

  while ((put=write(fd, buf, len))<0)
    if (errno!=EINTR && errno!=EAGAIN)
      return -1;
  tino_FATAL(put>len);
  return put;
}

/* Write everything, ignore signals (EINTR)
 * returns:
 * -1 on error or short write
 * else bytes written
 * (note that this always is len)
 */
static int
tino_write(int fd, const char *buf, size_t len)
{
  int	pos, put, cnt;

  /* Actually there is nothing like EOF on write.  However there are
   * buggy implementations out there where you might see a return of 0
   * instead of -1 if a write is interrupted at the wrong time.  So we
   * have a little grace and a retry count in case 0 is returned by
   * write.
   */
  cnt	= 10;
  for (pos=0; pos<len; )
    {
      put	= tino_write_step(fd, buf+pos, len-pos);
      if (put<=0)
	{
	  if (put<0 || --cnt<0)
	    return -1;
	  continue;
	}
      pos+=put;
      tino_FATAL(pos>len);
    }
  return pos;
}

static int
tino_readcmp(int fd, const char *buf, size_t len)
{
  int	pos, got, max;
  char	blk[BUFSIZ*16];

  for (pos=0; pos<len; pos+=got)
    {
      max	= len-pos;
      if (max>sizeof blk)
	max	= sizeof blk;
      got	= tino_read_step(fd, blk, max);
      if (got<=0)
	{
	  if (got)
	    return -1;
	  break;
	}

      if (got>max)
	tino_fatal("syscall defect, read more than allowed");

      if (memcmp(buf+pos, blk, got))
	return 1;
    }
  return 0;
}

#endif
