/* $Header$
 *
 * Additionally file helpers
 *
 * $Log$
 * Revision 1.4  2005-06-04 14:35:28  tino
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
 *
 */

#ifndef tino_INC_filetool_h
#define tino_INC_filetool_h

#include "fatal.h"
#include "alloc.h"
#include "str.h"

/* well, be prepared for windows
 */
#define	DRIVE_SEP_CHAR	0	/* untested	*/
#define	PATH_SEP_CHAR	'/'


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

  if (!buf)
    {
      if (max<BUFSIZ)
	max	= BUFSIZ;
      buf	= tino_alloc(max);
    }

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

static char *
tino_file_dirname(char *buf, size_t max, const char *name)
{
  int	offset;

  offset	= tino_file_dirfileoffset(name, 0);
  if (!buf)
    {
      if (max<BUFSIZ)
	max	= offset+1;
      buf	= tino_alloc(max);
    }
  if (max>offset)
    max	= offset+1;
  return tino_strxcpy(buf, name, max);
}

static char *
tino_file_filename(char *buf, size_t max, const char *name)
{
  int		offset;

  offset	= tino_file_dirfileoffset(name, 1);
  if (!buf)
    {
      if (max<BUFSIZ)
	max	= strlen(name+offset)+1;
      buf	= tino_alloc(max);
    }
  return tino_strxcpy(buf, name+offset, max);
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
#endif

#endif
