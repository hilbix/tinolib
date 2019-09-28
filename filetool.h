/* Additionally file helpers
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

#ifndef tino_INC_filetool_h
#define tino_INC_filetool_h

#include "file.h"
#include "fatal.h"
#include "alloc.h"
#include "str.h"

/** well, be prepared for windows
 */
#define	TINO_DRIVE_SEP_CHAR	0	/* untested	*/
#define	TINO_PATH_SEP_CHAR	'/'

/** Most of the functions in filetool.h which return char *
 * guarantee to return a buffer which is at least max
 * bytes long.  This routine creates such a buffer.
 *
 * To alloc the buffer as tight as possible just give
 * buf=0 and max=0
 *
 * ELSE THE RESULT CAN BE TRUNCATED, YOU HAVE BEEN WARNED!
 *
 * Now returns the new buffer if it has been allocated.
 */
static char *
tino_file_gluebufferOl(char **buf, size_t *max, size_t min)
{
  if (!*buf)
    {
      if (max && *max<min)
        *max	= min;
      *buf	= (char *)tino_allocO(max ? *max : min);
      return *buf;
    }
  return 0;
}

/** Extend the buffer from tino_file_gluebuffer()
 *
 * If the buffer is filled (min= *max) it is extended, too!
 */
static void
tino_file_gluebuffer_extendOl(char **buf, size_t *max, size_t min)
{
  if (*max <= min)
    {
      *max	= (min < *max+BUFSIZ ? *max+BUFSIZ : min);
      *buf	= (char *)tino_reallocO(*buf, *max);
    }
}

/** Append something to a gluebuffer
 *
 * Use as follows:
 *
 * name = tino_file_path_append(tino_file_path_append(NULL, filename), extension);
 * ...
 * tino_freeO(name);
 *
 * The inner (with NULL) allocates and returns a gluebuffer.  Only put
 * something in the first place, which can be realloc()ed AND it is
 * safe to SHRINK, too.
 */
static char *
tino_file_path_appendO(char *buf, const char *append)
{
  size_t	min, max;

  max	= 0;
  min	= (buf ? strlen(buf) : 0)+(append ? strlen(append) : 0)+2;
  if (tino_file_gluebufferOl(&buf, &max, min))
    *buf	= 0;
  tino_file_gluebuffer_extendOl(&buf, &max, min-1);
  if (append)
    tino_strxcat(buf, append, max);
  return buf;
}

/** This returns the root length of a path.
 * In contrast to the skip_root function this only returns
 * the offset of the drive root.  That is the index of the
 * character after the first / in the path.
 *
 * Return	OS	Example			Comment
 * 3		Windows	D:\\\\whatever
 * 0		Windows	D:whatever		Start to hate windows!
 * 1		Windows	\whatever
 * 1 (wrong!)	Windows	\\server\share\path	shall point to path
 * 1 (wrong?)	Windows	\.\c:			Better would be 3?
 * 1		Unix	////whatever
 * 1		Unix	/../whatever
 * 0		Unix	./whatever
 * 0		Unix	../whatever
 * 0		*	NULL
 *
 * UNC paths are not yet supported.
 * Windows raw drive extensions are not yet supported, too.
 */
static size_t
tino_file_is_rootedO(const char *path)
{
  size_t	off;

  if (!path)
    return 0;
  off	= 0;
#if TINO_DRIVE_SEP_CHAR
  if (isalpha(*path) && path[1]==TINO_DRIVE_SEP_CHAR)
    off	+= 2;
#endif
  if (path[off]==TINO_PATH_SEP_CHAR)
    return off+1;
  return 0;
}

/** Glue together some path and name.
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
tino_file_glue_pathOi(char *buf, size_t max, const char *path, const char *name)
{
#if TINO_DRIVE_SEP_CHAR
  int		drive;
#endif
  size_t	len, offset, min;

  min	= (path ? strlen(path) : 0)+(name ? strlen(name) : 0)+2;
  tino_file_gluebufferOl(&buf, &max, min);

  offset	= 0;
#if TINO_DRIVE_SEP_CHAR
  drive	= 0;
  if (path && isalpha(*path) && path[1]==TINO_DRIVE_SEP_CHAR)
    {
      drive	= *path;
      path	+= 2;
    }
  if (name && isalpha(*name) && name[1]==TINO_DRIVE_SEP_CHAR)
    {
      drive	= *name;
      name	+= 2;
    }
  if (drive)
    {
      TINO_FATAL_IF(max<3);
      buf[0]	= drive;
      buf[1]	= TINO_DRIVE_SEP_CHAR;
      offset	= 2;
    }
#endif

  /* speedup if name is absolute
   */
  if (!name || name[0]!=TINO_PATH_SEP_CHAR)
    {
      buf[offset]	= 0;
      if (path)
        tino_strxcpy(buf+offset, path, max-offset);
      if (!name)
        return buf;

      len	= strlen(buf);
      if (len && buf[len-1]!=TINO_PATH_SEP_CHAR && len+1<max)
        buf[len++]	= TINO_PATH_SEP_CHAR;
      offset	= len;
    }
  tino_strxcpy(buf+offset, name, max-offset);
  return buf;
}

/** Return pointer to the dir or file part.
 * If file==0 then return offset of the /
 * If file!=0 then return offset to basename
 */
static size_t
tino_file_dirfileoffsetO(const char *buf, int file)
{
  size_t	i, offset;

  TINO_FATAL_IF(!buf);
  if (file)
    file	= 1;
  offset	= 0;
#if TINO_DRIVE_SEP_CHAR
  if (isalpha(*buf) && buf[1]==TINO_DRIVE_SEP_CHAR)
    offset	+= 2;
#endif
  for (i=offset; buf[i]; i++)
    if (buf[i]==TINO_PATH_SEP_CHAR)
      offset	= i+file;
  return offset;
}

/** Hunt for the next pathchar from an offset.
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
tino_file_pathcharO(const char *buf, int offset)
{
  TINO_FATAL_IF(!buf);
  if (offset<0)
    {
      offset	= 0;
#if TINO_DRIVE_SEP_CHAR
      if (isalpha(*buf) && buf[1]==TINO_DRIVE_SEP_CHAR)
	return 2;
#endif
    }
  while (buf[offset]==TINO_PATH_SEP_CHAR)
    offset++;
  while (buf[offset])
    if (buf[++offset]==TINO_PATH_SEP_CHAR)
      return offset;
  return -1;
}

/** Return the dirname of a path
 */
static char *
tino_file_dirnameOi(char *buf, size_t max, const char *name)
{
  size_t	offset;

  offset	= tino_file_dirfileoffsetO(name, 0);
  tino_file_gluebufferOl(&buf, &max, offset+1);
  if (max>offset)
    max	= offset+1;
  return tino_strxcpy(buf, name, max);
}

/** Return the filename (last component) of a path
 */
static char *
tino_file_filenameOi(char *buf, size_t max, const char *name)
{
  size_t	offset;

  offset	= tino_file_dirfileoffsetO(name, 1);
  tino_file_gluebufferOl(&buf, &max, strlen(name+offset)+1);
  return tino_strxcpy(buf, name+offset, max);
}

/* Convenience routine:  Return filename (allocated)
 */
static char *
tino_file_filename_allocO(const char *name)
{
  return tino_file_filenameOi(NULL, (size_t)0, name);
}

/* Convenience routine:  Return filename (allocated)
 */
static char *
tino_file_dirname_allocO(const char *name)
{
  return tino_file_dirnameOi(NULL, (size_t)0, name);
}

/** Return the pointer to the filename part of a path
 */
static const char *
tino_file_filenameptr_constO(const char *path)
{
  return path+tino_file_dirfileoffsetO(path, 1);
}

/** Return the pointer to the filename part of a path
 */
static char *
tino_file_filenameptrO(char *path)
{
  return (char *)tino_file_filenameptr_constO(path);
}

/** Create a directory subtree for a filepart
 *
 * Returns:
 * -1	could not create directory
 * 0	path was present or no action taken
 * 1	directory or path created
 */
static int
tino_file_mkdirs_forfileE(const char *path, const char *file)
{
  size_t	minoffset;
  int		offset;
  char		*name;

  minoffset	= path ? strlen(path) : 0;
  name		= tino_file_glue_pathOi(NULL, 0, path, file);
  offset	= tino_file_dirfileoffsetO(name, 0);
  if (offset<0 || (unsigned)offset<=minoffset)
    {
      tino_freeO(name);
      return 0;
    }
  name[offset]	= 0;
  if (!tino_file_notdirE(name))
    {
      tino_freeO(name);
      return 0;
    }

  /* We do not have a directory, so we must create it.
   *
   * First, walk up the path until we can creat a directory.
   */
  while (tino_file_mkdirE(name))
    {
      if (errno!=ENOENT)
        {
	  tino_freeO(name);
	  return -1;
        }
      offset	= tino_file_dirfileoffsetO(name, 0);
      if (offset<0 || (unsigned)offset<=minoffset)
	{
	  tino_freeO(name);
	  return -1;
	}
      name[offset]	= 0;
    }
  
  /* Until we have reached the last component,
   * walk down the path and create the directory
   */
  for (;;)
    {
      tino_freeO(name);
      /* Rebuild the buffer
       *
       * Probably this can be done by poking TINO_PATH_SEP_CHAR back again,
       * but leave this to future optimizations.
       */
      name	= tino_file_glue_pathOi(NULL, 0, path, file);
      offset	= tino_file_pathcharO(name, offset);
      if (offset<0)
	{
	  /* We have reached the end, the directory was created
	   */
	  tino_freeO(name);
	  return 1;
	}
      name[offset]	= 0;
      if (tino_file_mkdirE(name) &&
	  (errno!=EEXIST || tino_file_notdirE(name)))
	break;
    }
  tino_freeO(name);
  return -1;
}

/** Create a backup filename
 * This is "name.~#~" where # is something starting at 1
 * It is guaranteed that this name does not exist (except for
 * race conditions).
 *
 * As I always handle hundreds of thousands of files
 * this must have O(ld(n)) complexity to find a free name.
 */
static char *
tino_file_backupnameNi(char *buf, size_t max, const char *name)
{
  long		i;
  unsigned long	lower, upper, min;
  char		tmp[200];
  size_t	len;
  char		*alloc;

  TINO_FATAL_IF(!name);

  len	= strlen(name);
  alloc	= tino_file_gluebufferOl(&buf, &max, len+sizeof tmp);
  tino_strxcpy(buf, name, max);

  lower	= 1;	/* this always is a used slot + 1	*/
  upper	= 1;	/* this always is an unused slot	*/
  i	= 1;
  min	= 2;
  for (;;)
    {
      snprintf(tmp, sizeof tmp, ".~%ld~", i);
#if 0
      fprintf(stderr, "back: %s\n", tmp);
#endif
      if (len<max)
	tino_strxcpy(buf+len, tmp, max-len);
      if (tino_file_notexistsE(buf))
	{
	  /* We have a hole.
           * But perhaps we have skipped a lot
	   */
	  if (lower>=(unsigned)i)
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
	  if (i<=0)	/* on overflow so something desparate	*/
	    i	= ++min;
	  upper	= i;

	  /* However if the current number already did not fit
	   * in the buffer, this cannot change anything in future.
	   * So bail out.
	   */
	  if (i<1 || len+strlen(tmp)>=max)
	    {
	      tino_freeO(alloc);
	      return 0;
	    }
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

/** Skip the root of a path.
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
tino_file_skip_root_constN(const char *path)
{
  if (!path)
    return 0;

#if TINO_DRIVE_SEP_CHAR
  if (isalpha(*path) && path[1]==TINO_DRIVE_SEP_CHAR)
    path	+= 2;
#endif
  for (;;)
    {
      if (*path==TINO_PATH_SEP_CHAR)
	path++;
      else if (*path=='.' && path[1]==TINO_PATH_SEP_CHAR)
	path+=2;
      else if (*path=='.' && path[1]=='.' && path[2]==TINO_PATH_SEP_CHAR)
	path+=3;
      else
	return path;
    }
}

static char *
tino_file_skip_rootN(char *path)
{
  return (char *)tino_file_skip_root_constN(path);
}

/**********************************************************************/
/* Some more complex functions
 */

/** Wrapper around getcwd() with suitable allocated buffer.
 *
 * Usually you call this as getcwd(NULL, 0) to allocate the buffer.
 *
 * Will return NULL on error or buffer is not big enough (ERANGE).
 */
static char *
tino_file_getcwd_bufN(char *buf, size_t max)
{
  char	*ret;

  if (buf)
    return getcwd(buf, max);

  /* Allocated case (buf==0)	*/

  /* According to doc get_current_dir_name() can return the environment
   * value, which might be different to what you expect.
   * So for stability we have to do it our own.
   */
  tino_file_gluebufferOl(&buf, &max, pathconf(".", _PC_PATH_MAX));
  while ((ret=getcwd(buf, max))==0 && errno==ERANGE)
    tino_file_gluebuffer_extendOl(&buf, &max, max+max);
  return tino_free_return_bufN(ret, buf);
}

/** Convenience function for getcwd() with allocated buffer
 *
 * Returns NULL on error
 */
static const char *
tino_file_getcwdO(void)
{
  return tino_file_getcwd_bufN(NULL, 0);
}

/** Return readlink() with a suitable buffer.
 *
 * This allocates the buffer big enough if buf is given as NULL and
 * always reaturns a 0 terminated buffer (in contrast to readlink()).
 * Only returns NULL if buffer is given and not big enough.
 *
 * Returns NULL on error (or buffer is not big enough: ERANGE)
 *
 * I hate that this isn't compatible to readlink().
 * My convention is: Return values are always the first parameters.
 */
static char *
tino_file_readlink_bufE(char *buf, size_t len, const char *file)
{
  size_t	n;
  char		*alloced;

  alloced	= 0;
  if (!buf)
    alloced	= tino_file_gluebufferOl(&buf, &len, pathconf(file, _PC_PATH_MAX));

  while ((n=TINO_F_readlink(file, buf, len))>0 && (unsigned)n>=len)
    {
      if (!alloced)
	{
	  errno	= ERANGE;
	  return 0;
	}
      tino_file_gluebuffer_extendOl(&buf, &len, n);
    }
  if (n<0)
    {
      tino_freeO(alloced);
      return 0;
    }
  buf[n]	= 0;
  return buf;
}

/** Convenience routine to readlink() with allocated buffer
 */
static const char *
tino_file_readlinkE(const char *file)
{
  return tino_file_readlink_bufE(NULL, 0, file);
}

/** This returns the realpath() of paths.  It works for nonexistent
 * paths, too (such that you can create missing directories or
 * filenames on the fly).
 *
 * This is braindeadly implemented, but the realpath() in the lib is
 * even more braindead.  realpath("unknown file", BUF) returns NULL
 * but BUF is set to the realpath!  However as this is an undocumented
 * behavior, I cannot rely on it.
 * Conclusion: I had to write my own implementation.
 *
 * Note that in contrast to glibc this skips a call to the getcwd()
 * kernel function if the current directory isn't needed.
 */
static char *
tino_file_realpath_cwdEl(char **buf, size_t *len, const char *file, const char *cwd, int level, int *errstate)
{
  size_t	tmplen;
  char		*tmp;
  size_t	i, off;

  if (level>256)
    {
      errno	= ELOOP;
      return 0;
    }
  tmp	= 0;
  tmplen= 0;
  if ((i=tino_file_is_rootedO(file))>0)
    {
      tino_file_gluebufferOl(&tmp, &tmplen, i+1);
      tino_strxcpy(tmp, file, i+1);
      off	= i;
    }
  else if (cwd)
    {
      off	= strlen(cwd);
      tino_file_gluebufferOl(&tmp, &tmplen, off+1);
      strncpy(tmp, cwd, off);
    }
  else
    {
      tmp	= tino_file_getcwd_bufN(NULL, 0);
      tmplen	= strlen(tmp);
      off	= tmplen;
    }
  *errstate	= 0;
  while (file[i])
    {
      tino_file_stat_t	st;
      size_t		j;
      const char	*lnk;
      char		*tmp2;
      size_t		tmp2len;

      if (*errstate>0)
	{
	  errno	= ENOTDIR;
	  return tino_free_return_bufN(0, tmp);	/* we hit something weird */
	}
      /* Skip / and ./
       */
      if (file[i]==TINO_PATH_SEP_CHAR ||
	  ( file[i]=='.' && ( file[i+1]==0 || file[i+1]==TINO_PATH_SEP_CHAR ) ))
	{
	  i++;
	  continue;
	}
      /* On .. go up one level
       */
      if (file[i]=='.' && file[i+1]=='.' && ( file[i+2]==0 || file[i+2]==TINO_PATH_SEP_CHAR ))
	{
	  tmp[off]	= 0;
	  off		= tino_file_dirfileoffsetO(tmp, 0);
	  i		+= 2;
	  *errstate	= 0;
	  continue;
	}

      /* Append the name
       */
      for (j=i; file[++j] && file[j]!=TINO_PATH_SEP_CHAR; );
      tino_file_gluebuffer_extendOl(&tmp, &tmplen, off+2+j-i);
      if (!off || tmp[off-1]!=TINO_PATH_SEP_CHAR)	/* case / (root dir)	*/
	tmp[off++]	= TINO_PATH_SEP_CHAR;
      strncpy(tmp+off, file+i, j-i); 
      off		+= j-i;
      i			= j;

      /* Check the stat()s of the path
       */
      if (!*errstate && ( tmp[off]=0, tino_file_lstatE(tmp, &st) ))
	{
	  if (errno!=ENOENT)
	    return tino_free_return_bufN(0, tmp);	/* we hit something weird */
	  *errstate	= -1;
	}
      if (*errstate)
	continue;

      /* If we added something which is no link just loop.
       */
      if (!S_ISLNK(st.st_mode))
	{
	  if (!S_ISDIR(st.st_mode))
	    *errstate	= 1;	/* We hit some non-directory	*/
	  continue;
	}

      /* get the link
       * Note that tmp[off] must be 0, as we are not in errstate
       */
      if ((lnk=tino_file_readlinkE(tmp))==0)
	return tino_free_return_bufN(0, tmp);

      /* Get the working directory again
       */
      off		= tino_file_dirfileoffsetO(tmp, 0);
      tmp[off]	= 0;

      /* Get the real path of the softlink
       *
       * This is recoursive to some extend
       */
      tmp2	= 0;
      tmp2len	= 0;
      if (!tino_file_realpath_cwdEl(&tmp2, &tmp2len, lnk, tmp, level+1, errstate))
	{
	  TINO_FATAL_IF(tmp2);
	  tino_free_constO(lnk);
	  return tino_free_return_bufN(0, tmp);
	}
      tino_free_constO(lnk);
      tino_freeO(tmp);

      /* Use the return value from the last recoursion as new path
       */
      tmp	= tmp2;
      tmplen	= tmp2len;
      off	= strlen(tmp);
    }
  if (!off)
    {
      tino_file_gluebufferOl(&tmp, &tmplen, 2);
      tmp[off++]= TINO_PATH_SEP_CHAR;
    }
  tmp[off]	= 0;
  tino_file_gluebufferOl(buf, len, off+1);
  tino_strxcpy(*buf, tmp, *len);
  if (*len<=off)
    {
      errno	= ERANGE;
      return tino_free_return_bufN(0, tmp);
    }
  return tino_free_return_bufN(*buf, tmp);
}

/** Return the realpath of a file in a suitable buffer
 *
 * See tino_file_realpath_cwd()
 */
static char *
tino_file_realpath_bufE(char *buf, size_t len, const char *file)
{
#if 1
  int	err;

  return tino_file_realpath_cwdEl(&buf, &len, file, NULL, 0, &err);
#else
  size_t	max;
  char		*tmp;

  max	= pathconf(file, _PC_PATH_MAX);
  tmp	= realpath(file, alloca(max));
  if (!tmp)
    return 0;
  max	= strlen(tmp)+1;
  tino_file_gluebuffer(&buf, &len, max);
  return tino_strxcpy(buf, tmp, len);
#endif
}

/** Convenience routine to tino_file_realpath() with allocated buffer
 */
static const char *
tino_file_realpathE(const char *file)
{
  return tino_file_realpath_bufE(NULL, 0, file);
}

/** Read a line from a file stream without trailing \n
 *
 * Optional \r are removed, too.
 *
 * returns:
 * -1 on error
 *  0 on EOF (no line)
 *  1 on a full line
 *  2 on a continuation line (this is, len was not long enough)
 */
static int
tino_file_gets(FILE *fd, char *ptr, size_t len)
{
  TINO_FATAL_IF(!ptr);
  TINO_FATAL_IF(len<2);
  if (!fd)
    return -1;
  while (tino_file_fgetsE(fd, ptr, len))
    {
      size_t	got;
      int	part;

      got	= strlen(ptr);
      if (!got)
	{
	  /* This can happen if you read from stdin and press ^D
	   */
	  continue;	/* or return 2?	*/
	}
      TINO_FATAL_IF(got>=len);
      part	= 1;
      while (got && (ptr[got-1]=='\n' || ptr[got-1]=='\r'))
	{
	  part	= 0;
	  got--;
	}
      ptr[got]	= 0;
      return part ? 2 : 1;
    }
  if (!tino_file_ferrorO(fd) && tino_file_feofO(fd))
    return 0;
  return -1;
}


/** Do a safe rename with an empty(!) helper directory.  You must
 * create the directory before.
 *
 * This has the sideffect, that it leaves a dead file (named after the
 * source) in the helper directory if it is interrupted.  Also be sure
 * that the helper directory is not accessed by any other processes
 * while this here runs.  The directory must be on the same filesystem
 * as the source (and destination), as the source cannot be renamed
 * else.
 *
 * It needs 3 syscalls to perform the action.
 *
 * First the source is renamed into the
 * This first does a rename into the helper directory (which is
 * assumed to be empty), then it does the hardlink and then it unlinks
 * in the helper directory.
 */
#if 0
static int
tino_file_rename_with_empty_helperEs(const char *helper_dir, const char *src, const char *new)
{
  const char	*tmp;
  size_t	offset;

  offset	= tino_file_dirfileoffsetO(src, 0);
  tmp		= tino_file_glue_pathOi(NULL, 0, helper_dir, src+offset);
  tino_file_gluebufferOl(&buf, &len,  pathconf(file, _PC_PATH_MAX));
  tmp	= tino_file
  err	= tino_file_rename_onlyEs(tmp, new);
  tino_freeO(tmp);
  return err;
}
#endif


/** Convenience: write a string
 */
static int
tino_file_write_sE(int fd, const char *s)
{
  return tino_file_write_allE(fd, s, strlen(s));
}


#ifdef TINO_TEST_UNIT
TESTCMP("/B", tino_file_glue_pathOi(NULL, 0, "/A", "/B"));
TESTCMP("/B", tino_file_glue_pathOi(NULL, 0, "A", "/B"));
TESTCMP("A/B", tino_file_glue_pathOi(NULL, 0, "A", "B"));
TESTCMP("/A/B", tino_file_glue_pathOi(NULL, 0, "/A", "B"));
TESTCMP("B", tino_file_glue_pathOi(NULL, 0, NULL, "B"));
TESTCMP("A", tino_file_glue_pathOi(NULL, 0, "A", NULL));
TESTCMP("", tino_file_glue_pathOi(NULL, 0, NULL, NULL));
TESTCMP("", tino_file_dirnameOi(NULL, 0, "/"));
TESTCMP("A", tino_file_dirnameOi(NULL, 0, "A/B"));
TESTCMP("", tino_file_filenameOi(NULL, 0, "A/"));
TESTCMP("B", tino_file_filenameOi(NULL, 0, "A/B"));
TESTCMP("A/B", tino_file_skip_root_constN("A/B"));
TESTCMP("A/B", tino_file_skip_root_constN("/A/B"));
TESTCMP(".../A/B", tino_file_skip_root_constN("//./..///.../A/B"));
#endif

#endif
