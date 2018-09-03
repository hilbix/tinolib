/* NOT READY YET * (will it ever be?)
 *
 * Generic IO layer.
 *
 * Note that error always is handled by callbacks.  If those are not
 * catched, then the error is printed and the program terminated.
 *
 * This is to be able to develop tools more quickly.
 * Currently only supports some limited output is implemented.
 *
 * This transparently enhances file descriptors as handed out from Unix,
 * as long as you stick using the _io_ and associated function (_get_
 * and _put_).  Several other files will be migrated to depend on this
 * in future.
 *
 * This is only the IO core.  Often you do not need to include it,
 * as it is automatically included by dependent sources.
 * So I try to keep down the bloat for example if you only need this
 * to capture some strings for convenience, but do not need all the
 * math support stuff of printf() variants.
 *
 * Note that output to a file is the same as output to a string,
 * which is the same as output to standard IO or output to sockets.
 * Everything here is unified like the Unix filesystem.
 *
 * IF YOU WANT TO DEPEND ON THIS, USE IT AS THE FIRST INCLUDE!
 * (with some common exceptions)
 *
 * In some distant future this will handle *all* IO the same way,
 * regardless if it is a file, directory, device, socket or telepathy.
 * YKWIM.
 *
 * Please read io.txt for more information about the conceptual idea.
 * However this file now is outdated and unsupported.
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_io_h
#define tino_INC_io_h

#if 0
#define TINO_FILE_EXCEPTION
#endif
#include "file.h"
#include "alloc.h"
#include "hash.h"
#include "strprintf.h"

#if 0
#include <sys/types.h>
#include <sys/socket.h>
#endif

#define	cDP	TINO_DP_io

/** Some important note with IO and alarm callbacks like from
 * tino_alarm():
 *
 * Never use IO from inside alarm callbacks which are also used
 * outside of the alarm callback in question.  While IO is pending
 * there might trigger some alarm callbacks, and thus you might
 * re-enter the IO code.  This may leave you with some unpredictable
 * results as you might move (extending them may move them) some
 * buffers which are currently output.
 *
 * Note that this is true for combined IO streams as well (so there
 * must not be shared any filters etc.).
 *
 * Detecting such unwanted re-entrancy and raising a deadlock-error
 * would not even bloat the core code, it would make things even less
 * reliable (you application spuriously would fail).  So the only real
 * way to fix that is to implement double buffering, this is, any
 * pending output is buffered before calling something else.
 *
 * On bigger IO this might slow things down unneccessarily, as in
 * advance there is no way to know if double-buffering is needed or
 * not.  This information must come from you.
 *
 * But if you have to provide this information anyway, the right fix
 * is to implement some "primitive IO double buffering module" which
 * can be added downstream for just those IO to file descriptors,
 * which really need it.
 *
 * For today this is not yet implemented. 90% of all such cases shall
 * give no problem anyway, and this will happen very seldom.  A good
 * chance to see this trouble is with progress meters to stderr when
 * the application is suspended while some error must be output to
 * stderr as well.
 *
 * However this is nothing new.  You must not call output from
 * signal-handlers as the result is unpredictable.  In a relaxed way
 * this still is true for tino_alarm(), too.
 */

/* Reserve IO 0 to 1023 for FDs 0 to 1023 */
/* Also note that negative IOs always are errors.
 */
#ifndef TINO_IO_RESERVED
#define TINO_IO_RESERVED 1024
#endif

/* Using more than 1 GB only to work with handles is insane, this way
 * we are protected against runaway handles this way.
 *
 * On 64 bit machines we can add some protection in the upper part of
 * the INT against the use of uninitalized handles.  In this case we
 * have: 32 bit protection + 32 bit number, both must fit together.
 *
 * On 32 bit we cannot do this.  24 bit only is 16 megahandles which
 * already is too low for a medium(!) use where each handle is an
 * object.  Note that INSANE_VALUE only protects against UNINITIALIZED
 * USE of a handles, not against allocating more than that.
 *
 * Memory of 2 GB permits about 26 bit, which is 64 Megahandles.  I
 * still consider this medium use (everything you can do at home today
 * cannot be a high demand).  So a real high demand only is possible
 * with 64 bit machines.
 */
#ifndef	TINO_IO_INSANE_VALUE
#define	TINO_IO_INSANE_VALUE	(1024*1024*1024/sizeof (struct tino_io))
#endif

/* Never even try to access or predict this.
 * It will be renamed/renumbered unconditionally!
 */
#define IO_CONS2(A,B)	A##B
#define IO_CONS(A,B)	IO_CONS2(A,B)
#define ION	123123
#define	IO	IO_CONS(tino_io_, ION)

/* Note that 0 is stdin, 1 is stdout and 2 is stderr.
 * The file handle is an int, as usual.
 *
 * And it has all properties of stdio.h, but a lot of things
 * are magic, so buffering can be added on the fly if needed.
 * This always happens if you start to use byte-IO which else
 * would be insanely inefficient.
 *
 * This way you can conveniently implement read-ahead
 * without the need to do anything complex.
 */
typedef struct tino_io *TINO_IO;
typedef const char *TINO_IO_ATOM;
static TINO_IO_ATOM	tino_io_atom(TINO_IO_ATOM *a, const char *str);

/* This is the buffer.
 *
 * data[0..(from-1)] is history
 * data[from..(to-1)] is filled with information
 * data[to..(max-1)] is free
 *
 * Everything else is handled outside.
 */
struct tino_io_buf
  {
    int			max, from /*r*/, to /*w*/;
    unsigned char	data[0];
  };

union tino_io_ext
  {
    void		*ptr;
    union tino_io_ext	*ext;
    struct tino_io_ind	*ind;
    struct tino_io_buf	*buf;
    void		(*write)(void *, const unsigned char *, size_t len);
    int			(*read)(void *, unsigned char *ptr, size_t len);
    int			(*ctl)(void *, TINO_IO_ATOM ctl, unsigned char *ptr, size_t len);
    int			(*err)(void *, TINO_VA_LIST);	/* 0:retry, 1:ignore/abort operation	*/
  };
struct tino_io_ind
  {
    /* Did not find a way to suppress the compile warning above when
     * specifying "union tino_io_ext (*ind)[2];", so need a struct to
     * encapsulate it.  Anyway, it's much clearer this way, too.
     *
     * It's an array to use pointering more easy (you cannot get a
     * pointer to a struct member easily when you only have a pointer
     * to another member).  Also the object might become a buffer or
     * similar in future.
     *
     * fn_ob[0].read(), fn_ob[0].write(), fn_ob[0].ctl(), fn_ob[0].err() etc.
     * fn_ob[1].ptr, fn_ob[1].buf, etc. 
     */
    union tino_io_ext	fn_ob[2];
  };
enum	/* tino_io->ext[TINO_IO_below]	*/
  {
    TINO_IO_OUT,	/* buffer	*/
    TINO_IO_IN,		/* buffer	*/
    TINO_IO_WRITE,
    TINO_IO_READ,
    TINO_IO_CTL,	/* control function	*/
    TINO_IO_ERR,	/* error, closed last	*/
  };

/* ext explained:
 *
 * ext need not be present.  If present, then
 * d is initialized to the number of elements in ext.
 *
 * All elements 0..(i-1) are indirect elements,
 * this are elements, which carry an object around.
 * All elements i..(d-1) are direct elements,
 * this means there is a function pointer stored.
 * If the function pointer is NULL, then the
 * standard function is executed.
 *
 * In the latter case the function get's passed the tino_io structure as first arg,
 * in the first case the function get's passed the pointer to the indirection.
 *
 * Note that the elements 0 and 1 are always indirect elements.
 * The available space (for write or read) is given in w and r respectively.
 * If w==0 or r==0 then the buffers might be missing as well.
 *
 * Indirection works such, that the pointer points to the indirection struct,
 * where the first element in this struct is the function pointer.
 * Everything else behind is not defined.
 *
 * There are following special calls:
 *
 * write(o, NULL, 0), read(o, NULL, 0) to close the IO.
 * also ctl(o, 0, NULL, 0), error(o, NULL).
 * ctl(o, 0, NULL, 0) actually should perform the close() action.
 * error(o, NULL) can do the cleanups.
 *
 * This allows for flush() and deallocation of internal buffers.
 * Afterwards this module does the cleanup (indirection block etc.).
 * Note for socket() type buffers, write() and read() can be closed
 * independently from each other (shutdown()) this way.
 *
 * write(o, (void *)1, 0) or read(o, (void *)1, 0) for flush().
 * This is, the upstream() wants you to flush() your held buffers,
 * possibly loosing efficiency (compression/decompression), etc.
 * This operation can be blocking (as always).
 *
 * Not that the buffers are automagically allocated if you use the
 * quick macros which work with buffers.
 */
struct tino_io
  {
    int			fd;	/* -2:used, -1:closed, 0:undef, >0:fd-1	*/
    const char		*name;	/* filename if present, allocated	*/

    short		w,r;	/* >=0:bytes room	*/
    short		i,d;	/* Indirects und Directs	*/
    union tino_io_ext	*ext;
  };

static struct
  {
    struct tino_io	*io;	/* io handle settings	*/
    int			count;	/* max number of allocated IO handles	*/
    int			minfree;/* lowest free handle, if ever	*/
    tino_hash_map	atoms;	/* Memory for the atoms	*/
  } IO;

#define	TINO_IO_NR(iop)	(int)(IO.io-(iop))	/* get the element #	*/


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* begin internal functions	*/

static int
tino_io_std_err(void *_o, TINO_VA_LIST list)
{
  TINO_IO	o=_o;
  int		ret;

  if (!o)
    ret	= tino_verr_new(list);
  else
    {
      const char	*str;

      str	= tino_str_vprintf(list);
      ret	= tino_err_new("%s: for IO handle %d, fd %d, name '%s'", str, TINO_IO_NR(o), o->fd, o->name);
      tino_free_constO(str);
    }
  return ret!=TINO_ERR_RET_RETRY;
}

/* Try to invoke the IO's error handler
 *
 * Call the std error handler if there is none.
 */
static int
tino_io_verr(int io, TINO_VA_LIST list)
{
  TINO_IO	o;

  o	= 0;
  if (io>=0 && io<IO.count)
    {
      union tino_io_ext	*e;

      o	= IO.io+io;
      if (TINO_IO_ERR<o->d && (e= &o->ext[TINO_IO_ERR])->ptr)
	{
	  if (TINO_IO_ERR<o->i)
	    {
	      e	= e->ind->fn_ob;
	      if (e[1].ptr)
		o	= e[1].ptr;
	    }
	  return e->err(o, list);
	}
    }
  return tino_io_std_err(o, list);
}

/* returns 0 for retry, 1 for ignore (do not retry)
 */
static int
tino_io_err(int io, const char *s, ...)
{
  tino_va_list	list;
  int		ret;

  tino_va_start(list, s);
  ret	= tino_io_verr(io, &list);
  /* usually only reached if you ignore/override the error
   *
   * Often the result then is unpredictable.
   */
  tino_va_end(list);
  return ret;
}

static void
tino_io_notyet(int io, const char *what)
{
  tino_io_err(io, "FTLIO100 not yet implemented: %s", what);
}

static int tino_io_fd(int, const char *);

#define	TINO_IO_CHECK(o,io,ret)					\
  if (io>=IO.count && io<TINO_IO_INSANE_VALUE)			\
    tino_io_fd(io, NULL);					\
  if (io<0 || io>=IO.count)					\
    {								\
      tino_io_err(io, "FTLIO101 fd %d out of range", io);	\
      return ret;						\
    }								\
  o	= IO.io+io;						\
  if (o->fd==-1)						\
    {								\
      tino_io_err(io, "ETLIO102F unused fd %d");		\
      return ret;						\
    }	/* fall through, ret is the error case	*/

static TINO_IO
tino_io_check(int io)
{
  TINO_IO o;

  TINO_IO_CHECK(o,io,0);
  return o;
}

static void tino_io_std_write(void *, const unsigned char *, size_t);
static int tino_io_std_read(void *, unsigned char *, size_t);
static int tino_io_std_ctl(void *, TINO_IO_ATOM, unsigned char *, size_t);
static int tino_io_std_err(void *, TINO_VA_LIST);
static union tino_io_ext tino_io_std[] =
  {
    { NULL },
    { NULL },
    { tino_io_std_write },
    { tino_io_std_read },
    { tino_io_std_ctl },
    { tino_io_std_err },
  };

/* There is a useful siedeffect, see the last line of this macro
 *
 * e will become the address of the functon-pointer, so e->write will do.
 * o is the pointer to the TINO_IO object before and (possibly) the indirection object afterwards.
 * n is the function type to look up.
 *
 * The indirection object is just an array of two values of the union.
 * The first one is the function, the second one is the object to call with the function.
 */
#define	TINO_IO_EXT(e,o,n,ret)							\
  if ( n>=o->d || (e= &o->ext[n])->ptr==0 )					\
    {										\
      if (n>=sizeof tino_io_std/sizeof *tino_io_std)				\
        {									\
          tino_io_err(TINO_IO_NR(o), "FTLIO103 unsupported function %d", n);	\
          return ret;								\
        }									\
      e	= &tino_io_std[n];							\
    }										\
  else if (n<o->i && (e=e->ind->fn_ob)[1].ptr)					\
    o	= e[1].ptr;	/* fetch the indirection object	*/

static union tino_io_ext *
tino_io_ext(TINO_IO o, int n)
{
  union tino_io_ext	*e;

  TINO_IO_EXT(e,o,n,0);
  return e;
}

static int
tino_io_std_ctl(void *_o, TINO_IO_ATOM ctl, unsigned char *buf, size_t max)
{
  TINO_IO	o=_o;

  switch (*ctl)
    {
      /* add some standard controls here	*/

    default:
      tino_io_err(TINO_IO_NR(o), "FTLIO103 unsupported control '%s'", ctl);
      break;
    }

  return -1;
}

/* DO NOT FORGET TO CALL tino_io_w() AFTERWARDS on TINO_IO_OUT
 * buffers.  Or you can set o->w=0, as you always can do this.
 */
static int
tino_io_buf_put(struct tino_io_buf *buf, const void *ptr, size_t max)
{
  int	n;

  /* Empty buffer?
   *
   * As good as we can we stay away from moving memory, instead we do
   * some additional syscalls.  This is by purpose, shuffling memory
   * is the task of the OS.
   */
  if (buf->from>=buf->to)
    {
      buf->to	= 0;
      buf->from	= 0;
    }

  if (max>(n=buf->max-buf->to))
    max	= n;

  memcpy(&buf->data[buf->to], ptr, max);
  buf->to	+= max;

  return max;
}

static void
tino_io_w(TINO_IO o)
{
  struct tino_io_buf	*buf;
  int			max;

  buf	= o->ext[TINO_IO_OUT].buf;
  if (buf->from>=buf->to)
    {
      buf->to	= 0;
      buf->from	= 0;
    }
  max	= buf->max-buf->to;
  if (max>32767)
    max	= 32767;
  o->w	=  max;

  cDP(("(%p) w=%d", o, o->w));
}

/* This does not look into buffers, as requiring this from the
 * downstream is too complex, so it must be part of the core.
 *
 * The standard read/write is as simple as it can be and straight
 * forward.  For more complex things there is ctl() like "receive a
 * buffer" and the like (perhaps there are optionally some more
 * standard functions).  Not today, so leave this to the future.
 */
static void
tino_io_std_write(void *_o, const unsigned char *ptr, size_t len)
{
  TINO_IO 	o=_o;
  int		io, fd;

  cDP(("(%p, %p, %d)", o,ptr,len));
  if (o->fd<0)
    {
      tino_io_err(TINO_IO_NR(o), "ETLIO104F write function not set");
      return;
    }
  /* This is correct: First check if this function is valid, then
   * check the args.
   */
  if (!ptr)
    return;	/* close() or flush()	*/

  /* No tino_file_*() routines, as we want to replace this
   *
   * There are systems where EINTR means death, as POSIX allows to
   * return -1 after data has been transferred.  *SIGH*
   *
   * We have no chance to detect this case when it happens.  To
   * protect against this case is slow and clumsy (it need things like
   * signal blocking etc.).
   *
   * For broken systems we sometimes can design a wrapper to replace
   * this standard routines here.  Perhaps.  Whatever.
   *
   * For safety we assume that, for some reason, our object go away
   * while looping here.  This can happen because of alarm()
   * callbacks.  The only thing which must not change is the pointer,
   * as we have no chance to detect that.
   *
   * This can happen when we are blocked while we write something out,
   * and the callback appends to this buffer.  We can only protect
   * against this by some obscure buffer locking or double-buffering,
   * which would only bloat the core's code.
   *
   * So such an implementation is left to be implemented in some
   * downstream module later on.  For now I only can say: Never use an
   * IO from a callback while the IO may be active.
   *
   * tino_alarm() allows that as it never re-enters alarm callbacks
   * while they are still active.  So just use your own IO handle from
   * each callback and you are set.
   */
  io	= TINO_IO_NR(o);
  fd	= o->fd;
  while (len)
    {
      int	got;

      got	= TINO_F_write(fd, ptr, len);
      if (got>0)
	{
	  if (got>len)
	    {
	      tino_io_err(io, "FTLIO105 syscall return value out of bounds: max=%llu got=%d", (unsigned long long)len, got);
	      break;
	    }
	  ptr	+= got;
	  len	-= got;
	}
      else if (!got)
	{
	  if (tino_io_err(io, "ETLIO106A EOF while writing?"))
	    break;
	}
      else if (errno!=EINTR)
	{
	  if (tino_io_err(io, "ETLIO107A low level write error"))
	    break;
	}

#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
}

static int
tino_io_std_read(void *_o, unsigned char *ptr, size_t len)
{
  TINO_IO	o=_o;
  int		io, fd;

  cDP(("(%p, %p, %d)", o,ptr,len));
  if (o->fd<0)
    {
      tino_io_err(TINO_IO_NR(o), "ETLIO108F read function not set");
      return 0;
    }
  /* This is correct: First check if this function is valid, then
   * check the args.
   */
  if (!ptr)
    return 0;	/* close() or flush()	*/

  io	= TINO_IO_NR(o);
  fd	= o->fd;
  while (len)
    {
      int	got;

      got	= TINO_F_read(fd, ptr, len);
      if (got>=0)
	{
	  if (got>len)
	    tino_io_err(io, "FTLIO109 syscall return value out of bounds: max=%llu got=%d", (unsigned long long)len, got);
	  return got;
	}
      if (errno!=EINTR)
	{
	  if (tino_io_err(io, "ETLIO110A low level read error"))
	    break;
	}

#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
  return 0;
}

static void
tino_io_ext_direct(TINO_IO o, int fn)
{
  if (fn>=o->d)
    {
      fn = fn-o->d+1;
      (void)TINO_REALLOC0_INC(o->ext, o->d, fn);
    }
}

/* Extend the ext array to the given indirect fn numbers
 *
 * Indirect means: What you see there is allocated and it is well
 * known what to do with it on deallocation.
 */
static void
tino_io_ext_indirect(TINO_IO o, int fn)
{
  if (o->i>fn)
    return;

  cDP(("(%p,%d)", o,fn));

  tino_io_ext_direct(o,fn);

  o->i	= fn+1;

  /* Wrap the indirection object around everthing which became
   * indirect now.
   *
   * SPARE OUT THE GIVEN FN (the new maximum)!
   */
  for (; o->d<fn; o->d++)
    if (o->d>TINO_IO_IN && o->d>TINO_IO_OUT && o->ext[o->d].ptr)
      {
	struct tino_io_ind	*ind;

	ind			= tino_alloc0O(sizeof *ind);

	cDP(("() wrap %d (%p) into %p", o->d, o->ext[o->d].ptr, ind));

	ind->fn_ob[0]		= o->ext[o->d];
	o->ext[o->d].ind	= ind;
      }
  cDP(("() i=%d d=%d", o->i, o->d));
}

static struct tino_io_buf *
tino_io_buf_new(int size)
{
  struct tino_io_buf	*buf;

  buf		= tino_alloc0O(size+sizeof *buf);
  buf->max	= size;
  return buf;
}


/* end internal functions	*/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* begin interface	*/

/* ctl explained:
 *
 * We have following standard functions:
 * write(), read() and close() (=write(NULL),read(NULL) in this sequence).
 * But how is the open() done?
 *
 * There are several ways to do this.  You can open() an fd and open the IO
 * with this fd in place.
 * Or you can create a new unspecific IO and attach that as the source
 * or sink or both.
 * This way you can control which functions are allowed.
 * 
 * Or, if there is a matching module, you can use the ctl call to perform
 * something which is like an open().  The latter is preferred, because
 * this can be handled transparent to the caller.  For example the open()
 * itself can be done on a remote system or even worse.
 *
 * ctl() also is a way to query for features or set some options as far
 * as these options are supported.
 */

/* To just flush what is buffered but do flush the downstream, use
 * tino_io_write(io, NULL, 1);
 *
 * For small sizes this should just add to the buffer and not write
 * anything.  Leave this optimization to the future.
 */
static void
tino_io_write(int io, const void *ptr, size_t len)
{
  while (len)
    {
      TINO_IO			 o,oo;
      union tino_io_ext		*e;
      struct tino_io_buf	*buf;
      int			n;

      /* Note that if we push data or buffers downstream, our current
       * buffer or any other pointers may go away.  So we must check
       * them for each iteration.
       */

      cDP(("(%d,%p,%d)", io,ptr,len));

      TINO_IO_CHECK(o,io,);

      /* Do the buffering.
       *
       * Short writes with enough room in the buffer are buffered
       * to skip system calls.
       *
       * XXX TODO: If there is a "reblocking" flag, always do it via
       * buffer.
       */
      buf	= 0;
      if (o->i>TINO_IO_OUT && (buf=o->ext[TINO_IO_OUT].buf)!=0 &&
	  ptr && len<buf->max)
	{
	  n	=  tino_io_buf_put(buf, ptr, len);
	  o->w	=  0;	/* invalidate	*/

	  cDP(("() buffered %d", n));

	  len	-= n;
	  if (!len)	/* Buffered everything?	*/
	    return;
	  ptr	= ((char *)ptr)+n;
	}
      /* remember: still len!=0 */

      /* Are we still here?  This means something must be flushed to
       * out, so get the pointers to output object and output
       * function.
       */
      oo=o;
      TINO_IO_EXT(e,oo,TINO_IO_WRITE,);

      cDP(("() check %p=%p,%p", o,oo,e));
      if (buf)
	cDP(("() buf=%p %d %d", buf, buf->from, buf->to));

      /* Flush the buffer if there is something in it
       */
      if (buf && (n=buf->to-buf->from)>0)
	{
	  void	*tmp;

	  /* Optimization potential:
	   *
	   * Send buffer to downstream instead of copying it there.
	   * Leave this to the future.
	   *
	   */
	  tmp		=  buf->data+buf->from;
	  buf->from	+= n;
	  o->r		=  0;	/* invalidate	*/

	  /*
	   * Danger ahead.  The routine may require changes in the
	   * core structure, so all pointers may be invalidated
	   * afterwards (including our object).  So we cannot do
	   * anything afterwards instead of relooping completely.
	   */
	  e->write(oo, tmp, n);
	  continue;	/* reloop for the next write	*/
	}
      /* remember: still len!=0	*/

      /* Buffers are flushed, now do the write (if not flush only)
       */
      if (ptr)
	e->write(oo, ptr, len);
      return;
    }
}

static void
tino_io_flush_write(int io)
{
  tino_io_write(io, NULL, 1);
}

/* There is no buffering yet, as this is complex, see write()
 */
static void
tino_io_read(int io, void *ptr, size_t len)
{
  struct tino_io	*o;
  union tino_io_ext	*e;

  if (!ptr || !len)
    return;
  TINO_IO_CHECK(o,io,);
  TINO_IO_EXT(e,o,TINO_IO_READ,);
  e->read(o, ptr, len);
}

/* Try to fill some data into the buffer
 *
 * Warning: After calling this routine, all IO pointers may be
 * invalidated.
 */
static void
tino_io_fill(int io)
{
  tino_io_notyet(io, "fill");
}

/* Create or flush the write buffer if needed
 *
 * Warning: After calling this routine, all IO pointers may be
 * invalidated.
 */
static void
tino_io_prep(int io)
{
  for (;;)
    {
      TINO_IO	o;

      cDP(("(%d)", io));

      o	= IO.io+io;

      if (o->i<=TINO_IO_OUT)
	tino_io_ext_indirect(o, TINO_IO_OUT);

      if (!o->ext[TINO_IO_OUT].buf)
	o->ext[TINO_IO_OUT].buf	= tino_io_buf_new(BUFSIZ);

      tino_io_w(o);

      if (o->w>0)
	return;

      tino_io_write(io, NULL, 1);
    }
}

/* Character at a time quick macros.
 * Note that these do not have any checks, so if you break it, you loose.
 */

TINO_INLINE(int
tino_io_put(int io, unsigned char c))
{
  struct tino_io	*o;
  struct tino_io_buf	*buf;

  cDP(("(%d,%02x %c)", io, c, c>=' ' && c<127 ? c : '.'));

  o	= IO.io+io;
  if (o->w<=0)
    {
      tino_io_prep(io);
      o	= IO.io+io;
    }

  /* prep leaves us 1 byte free room, min.	*/
  if (o->w<1)
    return -1;	/* signal error condition	*/

  buf	= o->ext[TINO_IO_OUT].buf;
  buf->data[buf->to++]	= c;
  o->w--;

  return c;	/* signal written char	*/
}

TINO_INLINE(int
tino_io_get_eof(int io))
{
  struct tino_io	*o;
  struct tino_io_buf	*buf;
  unsigned char		c;

  o	= IO.io+io;
  if (o->r<=0)
    {
      tino_io_fill(io);
      o	= IO.io+io;
    }
  /* The buffer is now filled if not EOF	*/

  if (o->r<=0)
    return -1;	/* got EOF (or error)	*/

  buf	= o->ext[TINO_IO_IN].buf;
  c	= buf->data[buf->from++];
  o->r--;

  return c;
}

/* This is much like ioctl(), however it
 *
 * - error always is handled internally
 * - always returns some int
 * - may write something to PTR (to return something)
 * - len is never modified.
 *
 * In simple cases the answer is the int.
 * In more complex cases the ptr is a buffer which is filled with data.
 * In extreme complex cases, ptr is a pointer to a struct.
 *
 * This all is as free as possible to be extended as easy as possible.
 */
static int
tino_io_ctl(int io, TINO_IO_ATOM ctl, void *ptr, size_t len)
{
  struct tino_io	*o;
  union tino_io_ext	*e;

  TINO_IO_CHECK(o,io,-1);
  TINO_IO_EXT(e,o,TINO_IO_CTL,-1);
  return e->ctl(o, ctl, ptr, len);
}

static int
tino_io_vctl(int io, TINO_IO_ATOM ctl, TINO_VA_LIST list)
{
  return tino_io_ctl(io, ctl, list, sizeof *list);
}

static int
tino_io_pctl(int io, TINO_IO_ATOM ctl, ...)
{
  tino_va_list	list;
  int		ret;

  tino_va_start(list, ctl);
  ret	= tino_io_vctl(io, ctl, &list);
  tino_va_end(list);
  return ret;
}

/* Create an TINO_IO_ATOM out of a string
 *
 * The idea is like in X11.  However, here ATOMs are pointers, not
 * INTs.  This way atoms can be printed more easily - you do not need
 * any function to lookup the atom's name, it's already it's name.
 */
static TINO_IO_ATOM
tino_io_atom_get(const char *str)
{
  return tino_hash_add_key(&IO.atoms, str, strlen(str))->key;
}

static TINO_IO_ATOM
tino_io_atom(TINO_IO_ATOM *a, const char *str)
{
  if (!*a)
    *a	= tino_io_atom_get(str);
  return *a;
}

static tino_file_size_t
tino_io_seek_type(int io, tino_file_size_t pos, TINO_IO_ATOM a)
{
  tino_io_read(io, NULL, 1);	/* flush input	*/
  tino_io_write(io, NULL, 1);	/* flush output	*/
  if (tino_io_ctl(io, a, &pos, sizeof pos))
    return -1;
  return pos;
}

/** Convenience routine
 *
 * Run something like a standard seek() operation.
 */
static tino_file_size_t
tino_io_seek(int io, tino_file_size_t pos)
{
  static TINO_IO_ATOM	a;

  return tino_io_seek_type(io, pos, tino_io_atom(&a, "SEEK_SET"));
}

static tino_file_size_t
tino_io_seek_end(int io, tino_file_size_t pos)
{
  static TINO_IO_ATOM	a;

  return tino_io_seek_type(io, pos, tino_io_atom(&a, "SEEK_END"));
}

static tino_file_size_t
tino_io_seek_rel(int io, tino_file_size_t pos)
{
  static TINO_IO_ATOM	a;

  return tino_io_seek_type(io, pos, tino_io_atom(&a, "SEEK_REL"));
}

/* Reserve a generic IO-handle as a file handle.
 * Reservation is needed only if you use normal
 * FDs in parallel to other defined FDs.
 *
 * If the FD already is taken from elsewhere,
 * (the first 1024 are never taken)
 * then you might get a remapped FD.
 * I can't help this.
 *
 * If the IO is unknown, then it is assumed to be
 * an FD and a proper structure is introduced, to
 * support the standard operations.
 *
 * Note that after a close() the fd is assumed
 * closed until re-opened with this call.
 */
static int
tino_io_fd(int fd, const char *name)
{
  TINO_IO	o;

  if (fd>=IO.count)
    {
      int	d=fd-IO.count+1;
      (void)TINO_REALLOC0_INC(IO.io, IO.count, d);
    }

  o		= IO.io+fd;
  o->fd		= fd;
  o->name	= name ? tino_strdupO(name) : 0;

  TINO_XXX;	/* just a dummy for now	*/

  cDP(("(%d)", fd));
  return fd;
}

/* Get an unbound IO handle.
 *
 * If the io is not attached anywhere it just errors
 * on any IO.  You can attach any function later.
 *
 * These handles do not start below TINO_IO_RESERVED.
 */
static int
tino_io_new(void)
{
  TINO_XXX;
  tino_io_notyet(-1, "new");
  return -1;
}


#if 0	/* OLD STUFF, will vanish soon */
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int
tino_io_new(const char *type)
{
  int	fd;
  int	i;

  /* Try to dup something
   */
  fd	= dup(2);
  if (fd<0)
    fd	= socket(PF_LOCAL, SOCK_DGRAM, 0);
  if (fd<0 || fd>0x10000000)
    tino_throw(TINO_EX_IO, "new");

  /* expand the working array
   */
  if (fd>=tino_io_fds)
    {
      int	i;

      i	= fd+256;
      I	= tino_realloc(I, i*sizeof *I);
      while (tino_io_fds<i)
	I[tino_io_fds++].fd	= -1;
    }

  /* The fd is already taken internally,
   * hunt for a free slot ..
   */
  if (I[fd].fd==-1)
    {
      000;
    }

  for (i=3; i<tino_io_fds; i++)
    {
      000;
    }
  000;
}

/* This can return -1 for error/not allocated,
 * or -2 if fd was closed, perhaps due to fd shortage.
 *
 * Usually tino_io_fd(x)==x.
 * Sometimes it might not:
 * When you open special IO things, or when your system runs out of file handles.
 * tino_io is thought to be able to handle this case gracefully,
 * sometimes, in the future.
 */
static int
tino_io_fd(int io)
{
  if (io<0 || io>=tino_io_fds)
    return -1;
  return I[io].fd;
}

/* Tell if EOF or ERROR condition:
 * Returns:
 * 1 if EOF,
 * 0 if no EOF and OK,
 * -1 if error state.
 */
static int
tino_io_eof(int io)
{
  000;
  return 1;
}

/* Close IO:
 * Returns:
 * 0 if OK,
 * -1 if error state.
 */
static int
tino_io_close(int io)
{
  000;
  return -1;
}

/* Do some processing.
 * Return if something is processed.
 * Returns number of signals received (EINTR) while everything is processed.
 * This function blocks maximum timeout seconds.
 * With timeout=0 it does not block at all.
 * With timeout=-1 it does block forever.
 */
static int
tino_io_process(int timeout)
{
  sleep(timeout);
  000;
  return 0;
}

static int
tino_io_open(const char *s, const char *mode, ...)
{
  000;
  return -1;
}

static int
tino_io_set(int io, int token, const void *p, size_t len)
{
  000;
  return -1;
}

static int
tino_io_set_ul(int io, int token, unsigned long ul)
{
  return tino_io_set(io, token, &ul, sizeof ul);
}

static int
tino_io_token(const char *s)
{
  return tino_token(s)+N;
}

static void
tino_io_copy(int in, int out)
{
  000;
}
#endif

#undef IO
#undef ION
#undef cDP
#endif
