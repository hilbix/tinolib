/* Manage lists of directories with file helpers
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_dirs_h
#define tino_INC_dirs_h

#include "filetool.h"
#include "alloc.h"

struct tino_dirs
  {
    struct tino_dirs_list
      {
        int	cnt;
        char	**list;
      }	path, tmp;
  };

typedef struct tino_dirs TINO_DIRS;

static void
tino_dirs_list_free(struct tino_dirs_list *l)
{
  int	i;

  for (i=l->cnt; --i>=0; )
    tino_freeO(l->list[i]);
  tino_freeO(l->list);
  l->cnt	= 0;
  l->list	= 0;
}

static struct tino_dirs_list
tino_dirs_list_reset(struct tino_dirs_list *l)
{
  struct tino_dirs_list	r;

  r		= *l;
  l->cnt	= 0;
  l->list	= 0;
  return r;
}

static char *
tino_dirs_list_add(struct tino_dirs_list *l, char *s)
{
  l->list		= tino_reallocO(l->list, (l->cnt+1) * sizeof *l->list);
  l->list[l->cnt++]	= s;
  return s;
}

static TINO_DIRS *
tino_dirs_gc(TINO_DIRS *d)
{
  tino_dirs_list_free(&d->tmp);
  return d;
}

static TINO_DIRS *
tino_dirs_free(TINO_DIRS *d)
{
  tino_dirs_list_free(&d->path);
  tino_dirs_list_free(&d->tmp);
  return d;
}

static TINO_DIRS *
tino_dirs_new(void)
{
  TINO_DIRS	*d;

  d	= tino_alloc0O(sizeof *d);
  return d;
}

static char *
tino_dirs_tmp(TINO_DIRS *d, char *s)
{
  tino_dirs_list_add(&d->tmp, s);
  return s;
}

static char *
tino_dirs_args(TINO_DIRS *d, int args, va_list list, int is_file)
{
  int	i;
  char	*buf;

  buf	= tino_file_path_appendO(NULL, NULL);
  for (i=0; i<args; i++)
    {
      const char	*arg;
      char		*tmp;

      arg	= va_arg(list, const char *);
      if (!arg)
        {
          /* NULL encountered, so we ignore this	*/
          tino_freeO(buf);
          return 0;
        }
      if (is_file)
        buf	= tino_file_path_appendO(buf, arg);
      else
        {
          tmp	= tino_file_glue_pathOi(NULL, 0, buf, arg);
          tino_freeO(buf);
          buf	= tmp;
        }
    }
  return buf;
}

static TINO_DIRS *
tino_dirs_add_direct(TINO_DIRS *d, char *s)
{
  int	i;

  if (!s)
    return d;
  for (i=d->path.cnt; --i>=0; )
    if (!strcmp(s, d->path.list[i]))
      {
        tino_freeO(s);
        return d;
      }
  tino_dirs_list_add(&d->path, s);
  return d;
}

/* Add a directory to the list.
 * Arguments are joined as paths.
 * The given arguments must not be NULL
 */
static TINO_DIRS *
tino_dirs_add(TINO_DIRS *d, int args, ...)
{
  va_list	list;
  char		*buf;

  va_start(list, args);
  buf	= tino_dirs_args(d, args, list, 0);
  va_end(list);

  return tino_dirs_add_direct(d, buf);
}

static TINO_DIRS *
tino_dirs_prune(TINO_DIRS *d, int max)
{
  if (max<0)
    max	 = 0;
  while (d->path.cnt > max)
    tino_freeO(d->path.list[--d->path.cnt]);
  return d;
}

/* PATH1:PATH2
 * PATH3:PATH4
 * becomes
 * PATH1
 * PATH2
 * PATH3
 * PATH4
 */
static TINO_DIRS *
tino_dirs_expand_pathvar(TINO_DIRS *d)
{
  struct tino_dirs_list	l;
  int			i;

  l	= tino_dirs_list_reset(&d->path);
  for (i=0; i<l.cnt; i++)
    {
      char	*pos, *tmp;

      for (pos=l.list[i]; (tmp=strchr(pos, ':'))!=0; pos=tmp)
        {
          *tmp++ = 0;
          tino_dirs_add_direct(d, tino_strdupO(pos));
        }
      tino_dirs_add_direct(d, tino_strdupO(pos));
    }
  tino_dirs_list_free(&l);
  return d;
}

/* Append all the other DIRS
 */
static TINO_DIRS *
tino_dirs_append(TINO_DIRS *d, TINO_DIRS *a)
{
  int	i;

  for (i=0; i<a->path.cnt; i++)
    tino_dirs_add_direct(d, tino_strdupO(a->path.list[i]));
  return d;
}

/* Add some subdirectory to all list entries
 */
static TINO_DIRS *
tino_dirs_sub(TINO_DIRS *d, int args, ...)
{
  va_list		list;
  struct tino_dirs_list	l;
  int			i;
  char			*buf;

  buf	= 0;
  va_start(list, args);
  for (i=0; i<args; i++)
    buf	= tino_file_path_appendO(buf, va_arg(list, const char *));
  va_end(list);
  if (!buf || !*buf)
    return d;

  l	= tino_dirs_list_reset(&d->path);
  for (i=0; i<l.cnt; i++)
    tino_dirs_add_direct(d, tino_file_glue_pathOi(NULL, 0, l.list[i], buf));
  tino_dirs_list_free(&l);
  return d;
}

/* Get a path.
 *
 * returns NULL if no directories on stack or some argument is NULL
 */
static const char *
tino_dirs_path(TINO_DIRS *d, int args, ...)
{
  va_list	list;
  char		*buf, *ret;

  if (!d->path.cnt)
    return 0;

  va_start(list, args);
  buf	= tino_dirs_args(d, args, list, 1);
  va_end(list);

  if (!buf)
    return 0;

  ret	= tino_file_glue_pathOi(NULL, 0, d->path.list[0], buf);
  tino_freeO(buf);
  return tino_dirs_tmp(d, ret);
}

static TINO_DIRS *
tino_dirs_pull(TINO_DIRS *d, int n)
{
  int	i;

  if (n <= 0)
    return d;
  if (n >= d->path.cnt)
    {
      tino_dirs_list_free(&d->path);
      return d;
    }
  for (i=n; --i>=0; )
    tino_freeO(d->path.list[i]);
  d->path.cnt	-= n;
  memmove(d->path.list, d->path.list+n, d->path.cnt * sizeof *d->path.list);
  return d;
}

#endif

