/* Rotateable logfiles
 *
 * Copyright (C)2006-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_log_h
#define tino_INC_log_h

#include "filetool.h"
#include "ex.h"

#include <time.h>

static const char	*tino_log_filename;
static char 		*tino_log_debug_level;
#define TINO_LOG_SUBSYS_MAX_LEN	200

/* do not call this before the last fork
 */
static void
tino_log_vprintfO(const char *prefix, int err, TINO_VA_LIST list)
{
  FILE		*fd;
  struct tm	tm;
  time_t	tim;
  static pid_t	pid;

  fd	= stderr;
  if (!tino_log_filename || (*tino_log_filename && (fd=fopen(tino_log_filename, "a+"))==0))
    return;

  time(&tim);
  gmtime_r(&tim, &tm);
  if (!pid)
    pid	= getpid();
  fprintf(fd,
	  "%4d-%02d-%02d %02d:%02d:%02d %ld: ",
	  1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
	  tm.tm_hour, tm.tm_min, tm.tm_sec,
	  (long)pid);
  if (prefix)
    fprintf(fd, "%s: ", prefix);
  tino_vfprintf(fd, list);
  if (prefix)
    fprintf(fd, ": %s (%d)", strerror(err), err);
  fputc('\n', fd);
  if (fd==stderr)
    fflush(fd);
  else
    fclose(fd);
  return;
}

static void
tino_logO(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_log_vprintfO(NULL, 0, &list);
  tino_va_end(list);
}

static void
tino_log_errorO(const char *prefix, TINO_VA_LIST list, int err)
{
  tino_verror_std(prefix, list, err);
  tino_log_vprintfO(prefix, err, list);
}

static void
tino_log_errO(const char *prefix, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_log_errorO(prefix, &list, errno);
  tino_va_end(list);
}

static char *
tino_log_debug_match(char *enabled, const char *subsys, int level)
{
  return NULL;
}

static int
tino_log_debug_tmp_subsys(char subsys[TINO_LOG_SUBSYS_MAX_LEN], int *level, const char *buf)
{
  size_t	len;
  char		*p;

  /* split setting into parts separated by blanks	*/
  tino_strxcpy(subsys, buf, TINO_LOG_SUBSYS_MAX_LEN);
  p = strchr(subsys, ' ');
  if (p)
    p = 0;
  len = strlen(subsys);
  if (len<TINO_LOG_SUBSYS_MAX_LEN)
    {
      if (level)
        {
          int lv = -1;		/* all	*/

          /* extract level from subsys:level
           * gracefully handle subsys*:level, too
           */
          if ((p=strchr(subsys, ':'))!=0)
            {
              lv	= atoi(p+1);
	      *p = 0;
            }
          /* extract level from subsys*level
           */
          if ((p=strchr(subsys,'*'))!=0 && *++p)
            {
	      lv = atoi(p);
	      *p = 0;
            }

	  *level = lv;
        }
      return len;
    }

  errno = EINVAL;
  return -1;
}

/* Warning!  Internal function!  Strings are modified!
 *
 * Add a subsys to list which is a space separated list of
 * subsys or subsys:level or subsys*level
 *
 * If a narrower (lower) level is found, replace this.
 * If a narrower match (subsys*) is found, replace this.
 * Else add this to the end.
 */
static int
tino_log_debug_subsys_add(char **list, const char *subsys, int level)
{
  char	tmp[TINO_LOG_SUBSYS_MAX_LEN+1];
  char	*pos;
  int	len;

  /* remove all matching old entries
   */
  for (pos= *list; (pos = tino_log_debug_match(pos, subsys, -1))!=0; )
    {
      int	len, lv;

      len = tino_log_debug_tmp_subsys(tmp, &lv, pos);
      if (len<0)
        return -1;	/* cannot happen	*/

      if (level>=0 && lv>level)
	{
	  /* Current position is broader, keep it
	   */
	  pos	+= len;
	  continue;
	}

      if (!strcmp(tmp, subsys) && level==lv)
	return 0;	/* found same entry, take shortcut	*/

      /* Remove entry
       * but re-add our entry at the end
       */
      if (pos[len])
	{
	  strcpy(pos, pos+len+1);	/* delete by moving down	*/
	  continue;
	}

      if (pos>*list)
	*--pos = 0;			/* delete at end of list	*/
      else
	TINO_FREE_NULL(*list);		/* list became empty	*/
      break;
    }

  /* Add this entry again
   */
  len	= *list ? strlen(*list) : 0;
  *list	= tino_reallocO(*list, len+TINO_LOG_SUBSYS_MAX_LEN+1);
  if (level<0)
    tino_strxcpy(tmp, subsys, sizeof tmp);	/* well, we could optimize this */
  else
    {
      snprintf(tmp, sizeof tmp, "%s:%d", subsys, level);
      tmp[sizeof tmp-1] = 0;	/* just to be sure	*/
    }
  if (strlen(tmp)>=TINO_LOG_SUBSYS_MAX_LEN)
    {
      errno = EINVAL;
      return -1;
    }
  /* append	*/
  if (len)
    (*list)[len++] = ' ';
  strcpy((*list)+len, tmp);
  return 0;
}

/* Warning!  Internal function!  Strings are modified!
 *
 * Remove a subsys from the list (about list see above).
 * Note there are no holes, so it is removed only.
 *
 * Find all entries which match the level and
 * reduce them.
 * If they are reduced below level 0 delete.
 */
static int
tino_log_debug_subsys_delete(char **list, const char *subsys, int level)
{
  char	*pos;

  while (*list && (pos = tino_log_debug_match(*list, subsys, level))!=0)
    {
      char	tmp[TINO_LOG_SUBSYS_MAX_LEN];
      int	len, lv_ignore;

      len = tino_log_debug_tmp_subsys(tmp, &lv_ignore, pos);
      if (len<0)
        return -1;	/* cannot happen	*/

      if (pos[len])
        strcpy(pos, pos+len+1);	/* delete by moving down	*/
      else if (pos>*list)
        *--pos = 0;		/* delete at end of list	*/
      else
	TINO_FREE_NULL(*list);	/* list became empty	*/

      /* if it was a reduce, add it again	*/
      if (level>0)
        tino_log_debug_subsys_add(list, tmp, level-1);
    }
  return 0;
}

/** Define debugging output into logging (if supported):
 * subsys	to enable maximum debugging on subsys
 * subsys:level	with reduced level of detail
 * !subsys	to disable subsys
 * !subsys:lv	to disable level and above
 * subs*	partial matching
 * subs*level	partial matching to level
 * *		maximum debugging
 * *:level	restricted to level
 * !*		to disable all
 */
static const char *
tino_log_debuggingO(const char *setting)
{
  while (setting && *setting)
    {
      char	subsys[TINO_LOG_SUBSYS_MAX_LEN];
      int	len, level;

      len = tino_log_debug_tmp_subsys(subsys, &level, setting);
      if (len<0)
        return NULL;

      setting += len;
      if (*setting)
        setting++;	/* skip blank	*/

      /* now we have a subsys in subsys[] and the level	*/
      if ((*subsys=='!'
	   ? tino_log_debug_subsys_delete(&tino_log_debug_level, subsys+1, level)
	   : tino_log_debug_subsys_add(&tino_log_debug_level, subsys, level))<0)
	return NULL;
    }
  return tino_log_debug_level;
}

static void
tino_log_debugO(const char *subsys, int level, const char *s, ...)
{
  tino_va_list	list;
  char		prefix[100];

  if (!tino_log_debug_level || !*tino_log_debug_level ||
      !tino_log_debug_match(tino_log_debug_level, subsys, level))
    return;
  
  tino_va_start(list, s);
  snprintf(prefix, sizeof prefix, "DEBUG %s %d", subsys, level);
  tino_log_vprintfO(prefix, 0, &list);
  tino_va_end(list);
}

static void
tino_log_fileO(const char *name)
{
  if (tino_log_filename && *tino_log_filename)
    tino_free_constO(tino_log_filename);
  tino_log_filename	= 0;
  if (name)
    tino_log_filename	= (!*name || !strcmp(name, "-")) ? "" : tino_file_realpathE(name);
}

#endif
