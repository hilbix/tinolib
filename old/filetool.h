/* $Header$
 *
 * Additionally file helpers
 *
 * $Log$
 * Revision 1.1  2005-04-24 12:55:38  tino
 * started GAT support and filetool added
 *
 */

#ifndef tino_INC_filetool_h
#define tino_INC_filetool_h

#include "alloc.h"
#include "str.h"

/* well, be prepared for windows
 */
#define	DRIVE_SEP_CHAR	0
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
  int		alloced, offset;
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
  FATAL(max<3);
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

#ifdef TINO_TEST_UNIT
TESTCMP("/B", tino_file_glue_path(NULL, 0, "/A", "/B"));
TESTCMP("/B", tino_file_glue_path(NULL, 0, "A", "/B"));
TESTCMP("A/B", tino_file_glue_path(NULL, 0, "A", "B"));
TESTCMP("/A/B", tino_file_glue_path(NULL, 0, "/A", "B"));
TESTCMP("B", tino_file_glue_path(NULL, 0, NULL, "B"));
TESTCMP("A", tino_file_glue_path(NULL, 0, "A", NULL));
TESTCMP("", tino_file_glue_path(NULL, 0, NULL, NULL));
#endif

#endif
