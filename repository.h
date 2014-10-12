/* UNIT TEST FAILS *
 * NOT READY YET!
 *
 * Generic repository files.
 *
 * A repository file holds general repository information.  This here
 * is special in that it supports cryptographic checksums, such that
 * it can detect unwanted data changes outside of the application
 * defined method.
 *
 * Copyright (C)2008-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_repository_h
#define tino_INC_repository_h

#if 0

#include "alloc.h"

struct tino_repository
  {
    TINO_DATA		d;
    const char		*name, *type, *crypt;
  } *TINO_REPOS;

struct tino_repository_pos
  {

  } *TINO_REPOS_POS;

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static void
tino_repos_err(TINO_REPOS r, const char *name, ...)
{
  tino_va_list	list;

  fprintf(stderr, "error %s: ", r->name);
  tino_va_start(list, name);
  tino_vfprintf(stderr, &list);
  tino_va_end(list);
  exit(1);
}

/** Initialize the repository structure
 *
 * NAME		filename to work on
 * TYPE		some additional resource type which must match (NULL for any)
 * CRYPT	crypt initializer for secure checksumming (NULL to disable)
 */
static void
tino_repos_init(TINO_REPOS r, const char *name, const char *type, const char *crypt)
{
  char	buf[128];
  int	fd;

  r->name	= tino_strdupO(name);
  r->type	= tino_strdupO(type);
  r->crypt	= tino_strdupO(crypt);
  if ((fd=tino_file_open_createE(r->name, "r+b"))<0)
    tino_repos_err(r, "cannot open");
  tino_data_file(&r->d, fd);

  n	= tino_data_readA(&r->d, buf, 1, sizeof buf, fd);
  if (n==128)
    {
      /* ACDG repository	*/
      if (buf[0]!=0xac || buf[1]!=0xd6 || buf[2]!=0xce || buf[3]!=0x90 || buf[4]!=0x51 || buf[5]!=0x10 || buf[6]!=0xc9 || buf[7])
	tino_repos_err(r, "magic mismatch");
      if (strncmp(buf+8, type, 128-8-16))
	tino_repos_err(r, "submagic mismatch: %s", type);
      tino_repos_check(r, 0l, buf, 128);
    }
  else
    {
      000;
    }
  000;
}

/** Write all unwritten repository resources to disk
 */
static void
tino_repos_sync(TINO_REPOS r, )
{
  000;
}

/** Free all the internal repository resources
 */
static void
tino_repos_free(TINO_REPOS r)
{
  tino_repos_sync(r);
  000;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/** Set a string repository parameter
 *
 * Repository parameters are always kept in memory.  Be careful if you
 * set parameters which start with __, like __TYPE or __CRYPT.
 */
static void
tino_repos_str_set(TINO_REPOS r, const char *parm, const char *val)
{
  000;
}

/** Get a string repository parameter
 *
 * Returns NULL if parameter unknown
 */
static const char *
tino_repos_str_get(TINO_REPOS r, const char *parm)
{
  000;
}

#if 0
/** Get an integer repository parameter
 *
 * Note that integer repository parameters internally are stored as
 * string.
 *
 * Repository parameters are always kept in memory.  Be careful if you
 * set parameters which start with __, like __TYPE or __CRYPT.
 */
static void
tino_repos_nr_set(TINO_REPOS r, const char *parm, long val)
{
  000;
}

/** Get an integer repository parameter
 *
 * If a parameter is unknown, this is an error.
 */
static long
tino_repos_nr_get(TINO_REPOS r, const char *parm)
{
  000;
}
#endif


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/** Get a current IO position of resource
 */
static void
tino_repos_posget(TINO_REPOS r, TINO_REPOS_POS p)
{
  000;
}

/** Go back to current IO position of resource
 */
static void
tino_repos_posset(TINO_REPOS r, TINO_REPOS_POS p)
{
  p->r	= r;
  000;
}

/** Get the maximum repository number
 *
 * Try to read from a maximum repository
 */
static long
tino_repos_max(TINO_REPOS r)
{
  000;
  return 0;
}

/** Select a resource number
 *
 * Note that resource 0 cannot be read nor written.  Setting is OK to
 * use tino_repos_next()
 */
static void
tino_repos_nr(TINO_REPOS r, long nr)
{
  000;
}

/** Get the next used repository number
 */
static long
tino_repos_next(TINO_REPOS r)
{
  000;
  return 0;
}

/** Total size of the given resource
 *
 * Unknown resources have size -1
 */
static long
tino_repos_size(TINO_REPOS r)
{
  000;
  return 0;
}

/** Set the resource offset for IO
 *
 * Can be higher than tino_repos_size() to write past EOD
 */
static void
tino_repos_seek(TINO_REPOS r, long off)
{
  000;
}

/** Like tino_repos_seek(), but from current position
 *
 * You can skip negatively, too.
 */
static void
tino_repos_skip(TINO_REPOS r, long off)
{
  000;
}

/** Get the number of bytes left in current block
 *
 * Returns 0 on EOF
 * Returns N<0 for sparse area, so -N next bytes are NUL
 * Returns N>0 for the current block size
 */
static long
tino_repos_left(TINO_REPOS r)
{
  000;
  return 0;
}

/** Read from a resource
 *
 * Reading into sparse area fills with NULL.  Trying to access past
 * EOD is an error.
 */
static void
tino_repos_read(TINO_REPOS r, void *ptr, size_t len)
{
  000;
}

/** Write to a resource
 *
 * You can write past EOD to create sparse data
 */
static void
tino_repos_write(TINO_REPOS r, const void *ptr, size_t len)
{
  000;
}

#endif
#endif
