/* $Header$
 *
 * NOT YET IMPLEMENTED
 * UNIT TEST FAILS *
 *
 * IO wrapper: Simple wrapper for stacked IO operations
 * Copyright (C)2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 * 
 * This is a simple IO wrapper.  The idea is, to have one single
 * interface to open/read/write/close for any operation needed, such
 * that the access can be abstract to the calling function.
 *
 * Also there is a generic easy error handler, which processes all
 * errors.
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
 * Revision 1.3  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.2  2007/08/06 15:55:39  tino
 * make test now works as expected
 *
 * Revision 1.1  2005/06/28 20:10:28  tino
 * started to add IOW (IO wrapper)
 *
 */

#ifndef tino_INC_iow_h
#define tino_INC_iow_h

#include "file.h"
#include "fatal.h"
#include "alloc.h"

typedef struct tino_iow *TINO_IOW;

/* States
 * Only one BIT must be set
 */
#define	TINO_IOW_READ		0x0001
#define	TINO_IOW_WRITE		0x0002
#define	TINO_IOW_NEW		0x0010
#define	TINO_IOW_DELAYED	0x0020
#define	TINO_IOW_OPEN		0x0040
#define	TINO_IOW_SYNC		0x0040	/* same as open	*/
#define	TINO_IOW_CLOSE		0x0080

strict tino_iow_link
  {
    const char		*name;
    TINO_IOW (		*endpoint)(TINO_IOW, void *user);
    TINO_IOW (		*filter)(TINO_IOW, TINO_IOW attached);
  };

struct tino_iow
  {
    void	*user;
    unsigned	state;


    /* Useful parameters
     */
    int		fd;		/* used by the generic functions for IO	*/
    long long	cnt_read, cnt_write;
    int		signals;	/* signal counter	*/

    /* Set this to an error cause (static string) if returning an error.
     */
    const char	*cause;		/* error cause		*/

    /* Parameters to alter through internal interface
     * Do not manage it yourself!
     */
    const char	*name;		/* a name if set	*/
    void	*imp;		/* destruct on close	*/

    /* Global error handler, called if any error happens.
     */
    int		(*error)(TINO_IOW, const char *, va_list);

    /* Global signal handler.
     * Default is to ignore a number of signals, and if too often, call error
     */
    void	(*signal)(TINO_IOW);

    /* generic functions:
     * return the equivalent of open(),close()
     *
     * return <0 on error
     * return >=0 on ok
     * Return value is ignored!
     *
     * The routines must store the file id etc. in the structure, else it is lost!
     */
    int		(*open)(TINO_IOW, const char *);
    int		(*close)(TINO_IOW, const char *);

    /* generic functions:
     * return the equivalent of read(),write():
     * return <0 error
     * return 0  EOF
     * return >0 amount read/written
     *
     * Routines need not do signal handling!
     * Routines must not reaturn EINTR on error.
     */
    int		(*read)(TINO_IOW, void *, size_t max);
    int		(*write)(TINO_IOW, const void *, size_t max);

    /* Sync data, flushs internal buffers, too.
     */
    int		(*sync)(TINO_IOW);

    /* Some internal flags
     */
    int		alloc;		/* is allocated, must be freed by tino_iow_free()	*/
    const char	*delayed;	/* Delayed open data, open is delayed until first IO	*/
  };

/* INTERNAL ROUTINES, never rely on the layout of such a routine!
 */

static void
tino_iow_free_delayed(TINO_IOW w)
{
  tino_FATAL((!w->delayed));
  tino_free((void *)w->delayed);
  w->delayed	= 0;
}

static void
tino_iow_free_imp(TINO_IOW w)
{
  if (w->imp)
    tino_free(w->imp);
  w->imp	= 0;
}

static void
tino_iow_free_name(TINO_IOW w)
{
  if (w->name)
    free((char *)w->name);
  w->name	= 0;
}

static int
tino_iow_gen_error(TINO_IOW w, const char *err, va_list list)
{
  tino_verror("generic io wrapper fatal error", err, list, errno);
  TINO_ABORT(-1);
}

static void
tino_iow_verror(TINO_IOW w, const char *err, va_list list)
{
  w->error(w, err, list);
  tino_FATAL("registered tino_iow_error function returned, shall not happen");
}

static void
tino_iow_error(TINO_IOW w, const char *err, ...)
{
  va_list	list;

  va_start(list, err);
  tino_iow_verror(w, err ? w->cause : err, list);
}

static void
tino_iow_gen_signal(TINO_IOW w)
{
  if (++w->signals>1000)
    tino_iow_error(w, "too many signals");
}

static int
tino_iow_gen_open(TINO_IOW w, const char *name)
{
  w->cause	= "open function not overloaded";
  return -1;
}

/* It's not that easy to process a close correctly
 */
static int
tino_iow_gen_close(TINO_IOW w, const char *why)
{
  if (w->fd==-1)
    {
      w->cause	= "file handle already closed";
      return -1;
    }
  while (tino_file_close_intr(w->fd))
    {
      if (errno!=EINTR && errno!=EAGAIN)
	return -1;
      w->signal(w);
    }
  w->fd	= -1;
  return 0;
}

static int
tino_iow_gen_read(TINO_IOW w, void *ptr, size_t max)
{
  return tino_file_read_intr(w->fd, ptr, max);
  int	got;

  while ((got=tino_file_read_intr(w->fd, ptr, max))<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	return -1;
      w->signal(w);
    }
  return got;
}

static int
tino_iow_gen_write(TINO_IOW w, const void *ptr, size_t max)
{
  int	got;

  while ((got=tino_file_write_intr(w->fd, ptr, max))<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	return -1;
      w->signal(w);
    }
  return got;
}

static int
tino_iow_gen_sync(TINO_IOW w)
{
  int	got;

  while ((got=tino_file_flush_fd(w->fd))<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	return -1;
      w->signal(w);
    }
  return 0;
}

static void
tino_iow_open_do(TINO_IOW w, const char *what)
{
  if (w->open(w, what)<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	tino_iow_error(w, NULL);
      w->signal(w);
    }
  w->state	= TINO_IOW_OPEN;
}

static void
tino_iow_start(TINO_IOW w, const char *cause, int allowed_state)
{
  tino_FATAL((!w->error || !w->signal || !w->open || !w->close || !w->read || !w->write || !w->sync));

  if (w->state&(w->state-1))
    tino_iow_error(w, "corrupt IOW state");

  if (!(w->state&allowed_state))
    {
      /* If we can solve the state problem with doing the delayed open,
       * do the open now.
       */
      if ((allowed_state&TINO_IOW_OPEN) && w->state&TINO_IOW_DELAYED)
	{
	  w->signals	= 0;
	  w->cause	= "open";
	  tino_FATAL((!w->delayed));
	  tino_iow_open_do(w, w->delayed);
	  tino_iow_free_delayed(w);
	}
      tino_iow_error(w, "wrong state %x for %s (allowed %x)", w->state, cause, allowed_state);
    }

  w->signals	= 0;
  w->cause	= cause;
}




/* Public Interface
 */

static TINO_IOW
tino_iow_init(TINO_IOW w, void *user)
{
  if (!w)
    {
      w		= tino_alloc(sizeof *w);
      w->alloc	= 1;
    }
  else
    w->alloc	= 0;
  w->state	= TINO_IOW_NEW;
  w->fd		= -1;
  w->cause	= 0;
  w->signals	= 0;
  w->user	= user;
  w->error	= tino_iow_gen_error;
  w->signal	= tino_iow_gen_signal;
  w->open	= tino_iow_gen_open;
  w->close	= tino_iow_gen_close;
  w->read	= tino_iow_gen_read;
  w->write	= tino_iow_gen_write;
  w->sync	= tino_iow_gen_sync;
  w->imp	= 0;
  w->name	= 0;
  return w;
}

static TINO_IOW
tino_iow_init_imp(TINO_IOW w, size_t imp, void *user)
{
  tino_iow_init(w, user);
  w->imp	= tino_alloc0(imp);
  return w;
}

/* delayed>0:	allocate string
 * delayed<0:	allocate pointer of size -delayed
 * 
 * w=tino_iow_open(tino_iow_socket(NULL, NULL), "host:port", 1);
 * w=tino_iow_open(my_init(tino_iow_init(NULL, user)), ptr_to_struct, -sizeof *ptr_to_struct);
 */
static TINO_IOW
tino_iow_open(TINO_IOW w, const void *what, int delayed)
{
  tino_iow_start(w, "open", TINO_IOW_NEW|TINO_IOW_CLOSE);
  if (delayed)
    {
      char	*tmp;

      if (delayed<0)
	delayed	= -delayed;
      else
	delayed	= strlen(what);
      tmp		= tino_alloc(delayed+1);
      memcpy(tmp, what, delayed);
      tmp[delayed]	= 0;
      w->delayed	= tmp;
      w->state		= TINO_IOW_DELAYED;
    }
  else
    tino_iow_open_do(w, what);
  return w;
}

static int
tino_iow_read(TINO_IOW w, void *ptr, size_t max)
{
  int	got;

  tino_iow_start(w, "read", TINO_IOW_OPEN|TINO_IOW_READ|TINO_IOW_WRITE);
  while ((got=w->read(w, ptr, max))<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	tino_iow_error(w, NULL);
      w->signal(w);
    }
  w->state	= TINO_IOW_READ;
  w->cnt_read	+= got;
  return got;
}

static int
tino_iow_write(TINO_IOW w, const void *ptr, size_t max)
{
  int	put;

  tino_iow_start(w, "write", TINO_IOW_OPEN|TINO_IOW_READ|TINO_IOW_WRITE);
  while ((put=w->write(w, ptr, max))<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	tino_iow_error(w, NULL);
      w->signal(w);
    }
  w->state	= TINO_IOW_WRITE;
  w->cnt_write	+= put;
  return put;
}

static TINO_IOW
tino_iow_sync(TINO_IOW w)
{
  tino_iow_start(w, "sync", TINO_IOW_OPEN|TINO_IOW_READ|TINO_IOW_WRITE);
  while (w->sync(w)<0)
    {
      if (errno!=EINTR && errno!=EAGAIN)
	tino_iow_error(w, NULL);
      w->signal(w);
    }
  w->state	= TINO_IOW_SYNC;
  return w;
}

static TINO_IOW
tino_iow_close(TINO_IOW w, const void *why)
{
  if (w->state&TINO_IOW_DELAYED)
    tino_iow_free_delayed(w);
  else
    {
      tino_iow_start(w, "close", TINO_IOW_OPEN|TINO_IOW_READ|TINO_IOW_WRITE);
      while (w->close(w, why)<0)
	{
	  if (errno!=EINTR && errno!=EAGAIN)
	    tino_iow_error(w, NULL);
	  w->signal(w);
	}
    }
  w->state	= TINO_IOW_CLOSE;
  return w;
}

static TINO_IOW
tino_iow_set_name(TINO_IOW w, const char *name)
{
  tino_iow_free_name(w);
  if (name)
    w->name	= tino_strdup(name);
  return w;
}

/* Deallocate space allocated by tino_iow_init()
 * Calls close() if needed.
 * Returns structure if it was not allocated internally.
 */
static TINO_IOW
tino_iow_free(TINO_IOW w)
{
  if (!w)
    return 0;

  if (!(w->state&(TINO_IOW_NEW|TINO_IOW_CLOSE)))
    tino_iow_close(w, NULL);

  tino_iow_free_imp(w);
  tino_iow_free_name(w);

  if (w->alloc)
    {
      free(w);
      return 0;
    }
  return w;
}

#endif
