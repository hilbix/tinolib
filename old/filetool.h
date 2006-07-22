/* $Header$
 *
 * Additionally file helpers
 *
 * Copyright (C)2004-2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.9  2006-07-22 23:47:44  tino
 * see ChangeLog (changes for mvatom)
 *
 * Revision 1.8  2006/07/22 17:41:21  tino
 * See ChangeLog
 *
 * Revision 1.7  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.6  2005/08/02 04:44:41  tino
 * C++ changes
 *
 * Revision 1.5  2005/08/01 17:35:39  tino
 * tino_file_filenameptr
 *
 * Revision 1.4  2005/06/04 14:35:28  tino
 * now passes unit test
 *
 * Revision 1.3  2005/04/25 23:07:01  tino
 * some new routines
 *
 * Revision 1.2  2005/04/24 13:44:11  tino
 * tino_file_notdir
 *
 * Revision 1.1  2005/04/24 12:55:38  tino
 * started GAT support and filetool added
 */

#ifndef tino_INC_filetool_h
#define tino_INC_filetool_h

#include "file.h"
#include "fatal.h"
#include "alloc.h"
#include "str.h"

/* well, be prepared for windows
 */
#define	DRIVE_SEP_CHAR	0	/* untested	*/
#define	PATH_SEP_CHAR	'/'

/* Most of the functions below which return char *
 * guarantee to return a buffer which is at least max
 * bytes long.  This routine creates such a buffer.
 *
 * To alloc the buffer as tight as possible just give
 * buf=0 and max=0
 *
 * ELSE THE RESULT CAN BE TRUNCATED, YOU HAVE BEEN WARNED!
 */
static void
tino_file_gluebuffer(char **buf, size_t *max, size_t min)
{
  if (!*buf)
    {
      if (*max<min)
        *max	= min;
      *buf	= (char *)tino_alloc(*max);
    }
}

/* Glue some path and name together.
 * If buffer is NULL it is allocated.
 *
 * This is intelligent:
 * If name is a full path, path is ignored.
 * Else only parts of path are taken for name.
 *
 * With windows it must become magic:
 * The drive prefix must be choosen from path and name.
 */
static char *
tino_file_glue_path(char *buf, size_t max, const char *path, const char *name)
{
#if DRIVE_SEP_CHAR
  int		drive;
#endif
  int		offset;
  size_t	len;

  tino_file_gluebuffer(&buf, &max, BUFSIZ);

  offset	= 0;
#if DRIVE_SEP_CHAR
  drive	= 0;
  if (path && isalpha(*path) && path[1]==DRIVE_SEP_CHAR)
    {
      drive	= *path;
      path	+= 2;
    }
  if (name && isalpha(*name) && name[1]==DRIVE_SEP_CHAR)
    {
      drive	= *name;
      name	+= 2;
    }
  TINO_FATAL_IF(max<3);
  if (drive)
    {
      buf[0]	= drive;
      buf[1]	= DRIVE_SEP_CHAR;
      offset	= 2;
    }
#endif

  /* speedup if name is absolute
   */
  if (!name || name[0]!=PATH_SEP_CHAR)
    {
      buf[offset]	= 0;
      if (path)
        tino_strxcpy(buf+offset, path, max-offset);
      if (!name)
        return buf;

      len	= strlen(buf);
      if (len && buf[len-1]!=PATH_SEP_CHAR && len+1<max)
        buf[len++]	= PATH_SEP_CHAR;
      offset	= len;
    }
  tino_strxcpy(buf+offset, name, max-offset);
  return buf;
}

static int
tino_file_dirfileoffset(const char *buf, int file)
{
  int	i, offset;

  TINO_FATAL_IF(!buf);
  if (file)
    file	= 1;
  offset	= 0;
#if DRIVE_SEP_CHAR
  if (isalpha(*buf) && buf[1]==DRIVE_SEP_CHAR)
    offset	+= 2;
#endif
  for (i=offset; buf[i]; i++)
    if (buf[i]==PATH_SEP_CHAR)
      offset	= i+file;
  return offset;
}

/* Hunt for the next pathchar from an offset.
 *
 * Start with offset -1 (to stop at drives)
 * or offset 0 (to only stop at path chars)
 * then feed the old offset until -1 is reached.
 *
 * ///unix/./path////etc
 *        ^ ^    ^
 *
 * c:\dir\file.c
 *   ^   ^
 *
 * Note that the first stop only is if you search with offset=-1 else
 * only the second and third stop will be taken:
 *
 * c:d\path\file.c
 *   ^^    ^
 */
static int
tino_file_pathchar(const char *buf, int offset)
{
  TINO_FATAL_IF(!buf);
  if (offset<0)
    {
      offset	= 0;
#if DRIVE_SEP_CHAR
      if (isalpha(*buf) && buf[1]==DRIVE_SEP_CHAR)
	return 2;
#endif
    }
  while (buf[offset]==PATH_SEP_CHAR)
    offset++;
  while (buf[offset])
    if (buf[++offset]==PATH_SEP_CHAR)
      return offset;
  return -1;
}

static char *
tino_file_dirname(char *buf, size_t max, const char *name)
{
  int	offset;

  offset	= tino_file_dirfileoffset(name, 0);
  tino_file_gluebuffer(&buf, &max, offset+1);
  if (max>offset)
    max	= offset+1;
  return tino_strxcpy(buf, name, max);
}

static char *
tino_file_filename(char *buf, size_t max, const char *name)
{
  int		offset;

  offset	= tino_file_dirfileoffset(name, 1);
  tino_file_gluebuffer(&buf, &max, strlen(name+offset)+1);
  return tino_strxcpy(buf, name+offset, max);
}

static const char *
tino_file_filenameptr(const char *path)
{
  return path+tino_file_dirfileoffset(path, 1);
}

/* Create a directory subtree for a filepart
 */
static int
tino_file_mkdirs_forfile(const char *path, const char *file)
{
  size_t	minoffset, offset;
  char		*name;

  minoffset	= strlen(path);
  name		= tino_file_glue_path(NULL, 0, path, file);
  offset	= tino_file_dirfileoffset(name, 0);
  if (offset<=minoffset)
    {
      free(name);
      return -1;
    }
  name[offset]	= 0;
  if (!tino_file_notdir(name))
    {
      free(name);
      return 0;
    }

  /* We do not have a directory, so we must create it.
   *
   * First, walk up the path until we can creat a directory.
   */
  while (!tino_file_mkdir(name))
    {
      offset	= tino_file_dirfileoffset(name, 0);
      if (errno!=ENOENT || offset<=minoffset)
	{
	  free(name);
	  return 1;
	}
      name[offset]	= 0;
    }
  
  /* Until we have reached the last component,
   * walk down the path and create the directory
   */
  for (;;)
    {
      minoffset	= offset;

      free(name);
      /* Rebuild the buffer
       *
       * Probably this can be done by poking PATH_SEP_CHAR back again,
       * but leave this to future optimizations.
       */
      name	= tino_file_glue_path(NULL, 0, path, file);
      offset	= tino_file_pathchar(name, offset);
      if (offset<0)
	{
	  /* We have reached the end, the directory was created
	   */
	  free(name);
	  return 0;
	}
      name[offset]	= 0;
      if (tino_file_mkdir(name) &&
	  (errno!=EEXIST || tino_file_notdir(name)))
	break;
    }
  free(name);
  return 1;
}

/* Create a backup filename
 * This is "name.~#~" where # is something starting at 1
 * It is guaranteed that this name does not exist (except for
 * race conditions).
 *
 * As I always handle hundreds of thousands of files
 * this must have O(ld(n)) complexity to find a free name.
 */
static char *
tino_file_backupname(char *buf, size_t max, const char *name)
{
  int		i, lower, upper;
  char		tmp[10];
  size_t	len;

  len	= strlen(name);
  tino_file_gluebuffer(&buf, &max, len+sizeof tmp);
  tino_strxcpy(buf, name, max);

  lower	= 1;	/* this always is a used slot + 1	*/
  upper	= 1;	/* this always is an unused slot	*/
  i	= 1;
  for (;;)
    {
      snprintf(tmp, sizeof tmp, ".~%d~", i);
      if (max<len)
	tino_strxcpy(buf+len, tmp, max-len);
      if (tino_file_notexists(buf))
	{
	  /* We have a hole.
           * But perhaps we have skipped a lot
	   */
	  if (lower>=i)
	    break;
	  upper	= i;
	  /* as lower<i and upper==i now
	   * we know that lower <= i < upper
	   * So there must be progress.
	   */
	  i	= (upper+lower)/2;
	  continue;
	}
      /* Search quadratic
       * We have a used slot
       */
      lower	= i+1;
      if (lower>upper)
	{
	  /* Upper points to an used slot,
	   * so increase it quadratic!
	   * i>=1, so upper=i+i >= lower=i+1
	   */
	  i	+= i;
	  upper	= i;

	  /* However if the current number already did not fit
	   * in the buffer, this cannot change anything in future.
	   * So bail out.
	   */
	  if (len+strlen(tmp)>=max)
	    return 0;
	  continue;
	}
      /* as lower==i+1 and i<upper
       * we know that lower <= i < upper
       * so there must be progress.
       */
      i	= (upper+lower)/2;
    }
  return buf;
}

/* Skip the root of a path.
 *
 * Usual roots are like "/" or "a:\" but I extend it to all leading
 * "redundant" or "parent" directories like . or ..
 *
 * So following is skipped:
 *
 * String		skipped		returns
 * "D:..\whatever"	"D:..\"		"whatever"
 * "./..//a"		"./..//"	"a"
 */
static const char *
tino_file_skip_root_const(const char *path)
{
  if (!path)
    return 0;

#if DRIVE_SEP_CHAR
  if (isalpha(*path) && path[1]==DRIVE_SEP_CHAR)
    path	+= 2;
#endif
  for (;;)
    {
      if (*path==PATH_SEP_CHAR)
	path++;
      else if (*path=='.' && path[1]==PATH_SEP_CHAR)
	path+=2;
      else if (*path=='.' && path[1]=='.' && path[2]==PATH_SEP_CHAR)
	path+=3;
      else
	return path;
    }
}

static char *
tino_file_skip_root(char *path)
{
  return (char *)tino_file_skip_root_const(path);
}

#ifdef TINO_TEST_UNIT
TESTCMP("/B", tino_file_glue_path(NULL, 0, "/A", "/B"));
TESTCMP("/B", tino_file_glue_path(NULL, 0, "A", "/B"));
TESTCMP("A/B", tino_file_glue_path(NULL, 0, "A", "B"));
TESTCMP("/A/B", tino_file_glue_path(NULL, 0, "/A", "B"));
TESTCMP("B", tino_file_glue_path(NULL, 0, NULL, "B"));
TESTCMP("A", tino_file_glue_path(NULL, 0, "A", NULL));
TESTCMP("", tino_file_glue_path(NULL, 0, NULL, NULL));
TESTCMP("", tino_file_dirname(NULL, 0, "/"));
TESTCMP("A", tino_file_dirname(NULL, 0, "A/B"));
TESTCMP("", tino_file_filename(NULL, 0, "A/"));
TESTCMP("B", tino_file_filename(NULL, 0, "A/B"));
TESTCMP("A/B", tino_file_skip_root_const("A/B"));
TESTCMP("A/B", tino_file_skip_root_const("/A/B"));
TESTCMP(".../A/B", tino_file_skip_root_const("//./..///.../A/B"));
#endif

#endif
