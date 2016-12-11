/* DATA processing handles
 *
 * With this wrapper you can either wrap TINO_BUF or files.  In future
 * more handlers will be added.  You can use it statically, but then
 * you must free the buffer yourself.
 *
 * Example:
 *
 * Circular: TINO_DATA *d1=tino_data_buf(NULL);
 * Bidirect: TINO_DATA *d2=tino_data_buf2(NULL, d1);
 *
 * TINO_DATA *d=tino_data_file(NULL, tino_file_open_create(name, O_APPEND, 0664));
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_data_h
#define tino_INC_data_h

#include "file.h"
#include "buf_printf.h"

#define	cDP	TINO_DP_data

typedef struct tino_data	TINO_DATA;

union tino_data_user
  {
    void	*ptr;
    TINO_DATA	*buf;
    FILE	*file;
    int		fd;
  };

#define tino_data_PASS(T,NAME)	\
TINO_INLINE(static union tino_data_user tino_data_user_##NAME(T NAME)) { union tino_data_user p; p.NAME = NAME; return p; }

tino_data_PASS(void *, ptr)
tino_data_PASS(TINO_DATA *, buf)
tino_data_PASS(FILE *, file)
tino_data_PASS(int, fd)

TINO_INLINE(static union tino_data_user tino_data_user_NULL()) { union tino_data_user p = { 0 }; return p; }

struct tino_data
  {
    TINO_BUF			buf;
    struct tino_data_handler	*handler;
    union tino_data_user	user;
    void			(*err)(TINO_DATA *, TINO_VA_LIST list);
    void			*allocated;	/* points to self if allocated	*/
  };

struct tino_data_handler
  {
    const char		*type;
    int			(*read)(TINO_DATA *, void *, size_t);
    int			(*write)(TINO_DATA *, const void *, size_t);
    int			(*sync)(TINO_DATA *, int sync);
    tino_file_size_t	(*pos)(TINO_DATA *);
    tino_file_size_t	(*size)(TINO_DATA *);
    tino_file_size_t	(*seek)(TINO_DATA *, tino_file_size_t pos);
    tino_file_size_t	(*seek_end)(TINO_DATA *);
    void		(*free)(TINO_DATA *);
    void		(*init)(TINO_DATA *, union tino_data_user);
  };

/** Set error handling function
 *
 * This function shall not return if you want to abort.  If it
 * returns, the error condition will be ignored.
 */
static void
tino_data_errfnO(TINO_DATA *d, void (*err)(TINO_DATA *, TINO_VA_LIST list))
{
  d->err	= err;
}

static void
tino_data_error(TINO_DATA *d, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  if (d && d->err)
    d->err(d, &list);
  else
    tino_vpexit((d->handler ? d->handler->type : "tino_data"), &list);
  tino_va_end(list);
}

static TINO_DATA *
tino_data_newO(union tino_data_user user)
{
  TINO_DATA	*d;

  d		= (TINO_DATA *)tino_alloc0O(sizeof *d);
  d->allocated	= d;
  d->user	= user;
  return d;
}

#pragma GCC diagnostic error "-Wall"
static void
tino_data_freeA(TINO_DATA *d)
{
  if (d->handler && d->handler->free)
    d->handler->free(d);
  d->handler	= 0;
  tino_buf_freeO(&d->buf);
  if (d->allocated /* avoid GCC 4.4.5 bug: && d->allocated==d */)
    free(d->allocated);
}

static TINO_DATA *
tino_data_handlerO(TINO_DATA *d, struct tino_data_handler *handler, union tino_data_user user)
{
  if (!d)
    d	= tino_data_newO(user);
  d->handler	= handler;
  if (handler && handler->init)
    handler->init(d, user);
  else
    d->user	= user;
  return d;
}

/** read data into buffer, never returns <0
 *
 * It may read less than requested!
 */
static int
tino_data_readA(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  if (!d || !d->handler || !d->handler->read)
    {
      tino_data_error(d, "read not defined");
      return 0;
    }
  while ((n=d->handler->read(d, ptr, max))<0 && errno==EINTR)
    {
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
  if (n<0)
    {
      tino_data_error(d, "general read error %d", n);
      return 0;
    }
  return n;
}

/** read data into buffer, never returns <0
 *
 * It reads the exact read count
 */
static void
tino_data_read_allA(TINO_DATA *d, void *_ptr, size_t len)
{
  size_t	pos;
  char		*ptr	= (char *)_ptr;

  for (pos=0; pos<len; )
    pos	+= tino_data_readA(d, ptr+pos, len-pos);
}

static tino_file_size_t
tino_data_posA(TINO_DATA *d)
{
  tino_file_size_t	l;

  if (!d || !d->handler || !d->handler->pos)
    {
      tino_data_error(d, "pos not defined");
      return 0;
    }
  l	= d->handler->pos(d);
  if (l==(tino_file_size_t)-1)
    tino_data_error(d, "cannot get data pos");
  return l;
}

static tino_file_size_t
tino_data_sizeA(TINO_DATA *d)
{
  tino_file_size_t	l;

  if (!d || !d->handler || !d->handler->size)
    {
      tino_data_error(d, "size not defined");
      return 0;
    }
  l	= d->handler->size(d);
  if (l==(tino_file_size_t)-1)
    tino_data_error(d, "cannot get data size");
  return l;
}

static void
tino_data_seekA(TINO_DATA *d, tino_file_size_t pos)
{
  if (!d || !d->handler || !d->handler->seek)
    {
      tino_data_error(d, "seek not defined");
      return;
    }
  if (d->handler->seek(d, pos)!=pos)
    tino_data_error(d, "general seek error");
}

static tino_file_size_t
tino_data_seek_endA(TINO_DATA *d)
{
  tino_file_size_t	l;

  if (!d || !d->handler || !d->handler->seek_end)
    {
      tino_data_error(d, "seek_end not defined");
      return 0;
    }
  l	= d->handler->seek_end(d);
  if (l==(tino_file_size_t)-1)
    tino_data_error(d, "cannot get data size");
  return l;
}

static tino_file_size_t
tino_data_size_genericA(TINO_DATA *d)
{
  tino_file_size_t	pos, len;

  if (!d || !d->handler || !d->handler->seek_end)
    {
      tino_data_error(d, "cannot seek to get size");
      return 0;
    }
  pos	= tino_data_posA(d);
  len	= tino_data_seek_endA(d);
  tino_data_seekA(d, pos);
  return len;
}

/** sync data buffers
 *
 * if sync is not set, syncing is done in background
 */
static void
tino_data_syncA(TINO_DATA *d, int sync)
{
  if (!d || !d->handler || !d->handler->sync)
    {
      tino_data_error(d, "sync not defined");
      return;
    }
  if (d->handler->sync(d, sync))
    tino_data_error(d, "general sync error");
}

/** write out data
 *
 * This writes all data all times, however some not flushed data may
 * remain in some internal buffers.
 */
static void
tino_data_writeA(TINO_DATA *d, const void *ptr, size_t len)
{
  size_t	pos;
  int		put;

  if (!d || !d->handler || !d->handler->write)
    {
      tino_data_error(d, "write not defined");
      return;
    }
  put	= -1;
  for (pos=0; pos<len; )
    {
      errno	= 0;
      if ((put=d->handler->write(d, ((const char *)ptr)+pos, len-pos))>0)
	pos	+= put;
      else if (!put || errno!=EINTR)
	break;
      else
	{
#ifdef TINO_ALARM_RUN
	  TINO_ALARM_RUN();
#endif
	}
    }
  if (pos!=len)
    tino_data_error(d, "general write error %d", put);
}

/* XXX TODO remove following sometimes in future	*/
#define tino_data_vsprintfA tino_data_vprintfA

/** Print out a string (varargs version).
 */
static void
tino_data_vprintfA(TINO_DATA *d, TINO_VA_LIST list)
{
  TINO_BUF	buf;

  tino_buf_initO(&buf);
  tino_buf_vprintfO(&buf, list);
  tino_data_writeA(d, tino_buf_getN(&buf), tino_buf_get_lenO(&buf));
  tino_buf_freeO(&buf);
}

/** Print out a string.
 */
static void
tino_data_printfA(TINO_DATA *d, const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_data_vprintfA(d, &list);
  tino_va_end(list);
}

/** Convenience routine (speedup)
 */
static void
tino_data_putsA(TINO_DATA *d, const char *s)
{
  tino_data_writeA(d, s, strlen(s));
}

/** Escape a buffer according to C escapes.
 *
 * This always escapes \ to \\ NUL to \000 and DEL to \177
 *
 * flags gives which sequences to escape.  Use 0xff to escape
 * everything in C excluding proper ASCII, use -1 for maximum escape.
 */
#define	TINO_C_ESCAPE_ALL	0xff
#define	TINO_C_ESCAPE_CONTROL	0x01	/* escapes everything below SPC	*/
#define	TINO_C_ESCAPE_SPACES	0x02	/* escapes \a \b \t \n \v \f \r	*/
#define	TINO_C_ESCAPE_QUOTES	0x04	/* escapes \" \'		*/
#define	TINO_C_ESCAPE_HICONTROL	0x40	/* escape from 0x80 to 0x9f	*/
#define	TINO_C_ESCAPE_HIGHER	0x80	/* escape from 0xa0 to 0xff	*/
#define	TINO_C_ESCAPE_SHELL	0x100	/* escapes \$ \`		*/
#define	TINO_C_ESCAPE_PHP	0x300	/* escapes \( \) \{ \} \[ \]	*//*includes SHELL!*/
#define	TINO_C_ESCAPE_PERCENT	0x400	/* escapes \%			*/
#define	TINO_C_ESCAPE_REGEX	0xC00	/* escapes \. \+ \? \* \_	*//*includes PERCENT!*/
#define	TINO_C_ESCAPE_ESC	0x1000	/* escapes \e for 033		*/
#define	TINO_C_DUB_SINGLE_A	0x01	/* escape ' to ''		*/
#define	TINO_C_DUB_SINGLE_B	0x02	/* escape ' to '\''		*/
#define	TINO_C_DUB_SINGLE_C	0x03	/* escape ' to '"'"'		*/
#define	TINO_C_DUB_PRINTF	0x04	/* escape % to %%		*/
#define	TINO_C_DUB_MAKE		0x08	/* escape $ to $$		*/
static void
tino_data_write_c_escapeA(TINO_DATA *d, const void *ptr, int len, int flags, int dub)
{
  const unsigned char	*s=ptr;
  int			pos, cnt, i, tmp;
  char			esc[0x100];	/* escape matrix: 0=print, 1=octal, 2=double, 3 to 31:special, 32 to ff: \x	*/
  static char		*special[] = { "'\\''", "'\"'\"'" };
  
  memset(esc, 0, sizeof esc);

  if (flags&TINO_C_ESCAPE_HIGHER)
    for (i=0x100; --i>=0xA0; esc[i]=1);
  if (flags&TINO_C_ESCAPE_HICONTROL)
    for (i=0x0A0; --i>=0x80; esc[i]=1);
  esc[0x7f]	=1;
  if (flags&TINO_C_ESCAPE_CONTROL)
    for (i=0x020; --i>=0x01; esc[i]=1);
  esc[0]	= 1;

  if (flags&TINO_C_ESCAPE_SPACES)
    {
      esc['\a']	= 'a';
      esc['\b']	= 'b';
      esc['\t']	= 't';
      esc['\n']	= 'n';
      esc['\v']	= 'v';
      esc['\f']	= 'f';
      esc['\r']	= 'r';
    }
  if (flags&TINO_C_ESCAPE_QUOTES)
    {
      esc['\"']	= '\"';
      esc['\'']	= '\'';
    }
  if (flags&TINO_C_ESCAPE_SHELL)
    {
      esc['$']	= '$';
      esc['`']	= '`';
    }
  if (flags&TINO_C_ESCAPE_PHP&~TINO_C_ESCAPE_SHELL)
    {
      esc['(']	= '(';
      esc[')']	= ')';
      esc['[']	= '[';
      esc[']']	= ']';
      esc['{']	= '{';
      esc['}']	= '}';
    }
  if (flags&TINO_C_ESCAPE_PERCENT)
    {
      esc['%']	= '%';	/* beware double below	*/
    }
  if (flags&TINO_C_ESCAPE_REGEX)
    {
      esc['.']	= '.';
      esc['+']	= '+';
      esc['?']	= '?';
      esc['*']	= '*';
      esc['_']	= '_';
    }
  if (flags&TINO_C_ESCAPE_ESC)
    {
      esc['\033']	= 'e';
    }
#define	TINO_C_DUB_SINGLE_A	0x01
#define	TINO_C_DUB_SINGLE_B	0x02
#define	TINO_C_DUB_SINGLE_C	0x03
  if ((tmp=dub & 3/*==TINO_C_DUB_SINGLE_C*/)!=0)
    {
      esc['\'']	= tmp+1;	/* 2, 3, 4, yes, this is deep black magic!	*/
    }
  if (dub&TINO_C_DUB_PRINTF)
    {
      esc['%']	= 2;
    }
  if (dub&TINO_C_DUB_MAKE)
    {
      esc['$']	= 2;
    }
  for (pos=-1, cnt=0; ++pos<len; )
    {
      char	e;

      if ((e=esc[s[pos]])==0)
	{
	  cnt++;
	  continue;
	}

      if (e==2)		/* doubled character	*/
	{
	  /* Just put out sequence including current character and let
	   * start the new sequence at the current character, too
	   */
	  tino_data_writeA(d, s+pos-cnt, cnt+1);
	  cnt	= 1;
	  continue;
	}

      /* send what we had up to here
       */
      if (cnt)
	tino_data_writeA(d, s+pos-cnt, cnt);

      if (e==1)		/* octal escape	*/
	tino_data_printfA(d, "\\%03o", s[pos]);

      else if (e>=0x20)	/* character escape \x	*/
	{
#if 0
	  if (e==s[pos])
	    {
	      /* Optimization in case the escape is the same
	       * character.  Hopefully we can skip one call to write.
	       */
	      tino_data_writeA(d, "\\", 1);
	      cnt	= 1;
	      continue;
	    }
#endif
	  /* Replacement backslash sequence
	   */
	  tino_data_printfA(d, "\\%c", e);
	}

      else		/* special sequence	*/
	tino_data_putsA(d, special[e-3]);

      cnt	= 0;	/* Do not repeat current character	*/
    }

  /* Transfer the last data piece
   */
  if (cnt)
    tino_data_writeA(d, s+pos-cnt, cnt);
}

#if 0
/** XML escape data
 *
 * There are two modes, either entity escape (this is replace "&<>
 * with &quot;&amp;&lt;&gt; respectively) or CDATA mode
 *
 * Cdata mode is used, when cdata is set to 1.  Note that this does
 * not properly convert into the ISO charset.
 */
static void
tino_data_write_xmlA(TINO_DATA d, const void *s, int len, int cdata)
{
  if (cdata)
    000;
#error
}
#endif

#if 0
/** Escape a string
 *
 * 'escape' will be doubled, "escaped" are characters which are
 * escaped by 'escape'
 */
static void
tino_data_puts_escapeAbug(TINO_DATA *d, const char *s, char escape, const char *escaped)
{
  int	loop;

  for (loop=0; *s; loop=1)
    {
      const char	*ptr;

      ptr	= strchr(s+loop, escape);
      if (escaped)
	{
	  const char	*tmp;

	  tmp	= strpbrk(s+loop, escaped);
	  if (!ptr || (tmp && tmp<ptr))
	    ptr	= tmp;
	}
      if (!ptr)
	{
	  tino_data_writeA(d, s, strlen(s));
	}
	break;
	ptr	= s+strlen(s);
      tino_data_writeA(d, s, ptr-s);
      s	= ptr;
    }
}
#endif


/**********************************************************************/

static int
tino_data_buf_readO(TINO_DATA *d, void *ptr, size_t max)
{
  return tino_buf_fetchO(&d->buf, ptr, max);
}

/* If we are connected to another buf write into the other buffer.
 */
static int
tino_data_buf_writeO(TINO_DATA *d, const void *ptr, size_t max)
{
  tino_buf_add_nO((d->user.buf ? &d->user.buf->buf : &d->buf), ptr, max);
  return max;
}

static void
tino_data_buf_freeO(TINO_DATA *d)
{
  if (!d->user.buf)
    return;
  d->user.buf->user.buf	= 0;
  d->user.buf		= 0;
}

static int
tino_data_buf_syncO(TINO_DATA *d, int sync)
{
  /* This is always synced	*/
  return 0;
}

struct tino_data_handler tino_data_buf_handler	=
  {
    "tino_data memory buffer",
    tino_data_buf_readO,
    tino_data_buf_writeO,
    tino_data_buf_syncO,
    0,
    0,
    0,
    0,
    tino_data_buf_freeO,
    0
  };

#define	TINO_DATA_BUF	(&tino_data_buf_handler)

/* A buffer connected to another buffer
 */
static TINO_DATA *
tino_data_buf2O(TINO_DATA *d, TINO_DATA *second)
{
  tino_FATAL(second && (second->handler!=TINO_DATA_BUF || second->user.buf));
  d	= tino_data_handlerO(d, TINO_DATA_BUF, tino_data_user_buf(second));
  if (second)
    second->user.buf	= d;
  return d;
}

/* Circular buffer
 */
static TINO_DATA *
tino_data_bufO(TINO_DATA *d)
{
  return tino_data_buf2O(d, NULL);
}


/**********************************************************************/

static int
tino_data_file_syncA(TINO_DATA *d, int sync)
{
  if (sync && tino_file_flush_fdE(d->user.fd))
    {
      tino_data_error(d, "file flush error fd %d", d->user.fd);
      return -1;
    }
  return 0;
}

static int
tino_data_file_readI(TINO_DATA *d, void *ptr, size_t max)
{
  int	n;

  n	= tino_file_readI(d->user.fd, ptr, max);
  if (n<0 && errno!=EINTR)
    {
      tino_data_error(d, "file read error fd %d", d->user.fd);
      n	= 0;
    }
  return n;
}

static int
tino_data_file_writeI(TINO_DATA *d, const void *ptr, size_t max)
{
  int	n;

  n	= tino_file_writeI(d->user.fd, ptr, max);
  if (n<0 && errno!=EINTR)
    {
      tino_data_error(d, "file write error fd %d", d->user.fd);
      n	= 0;
    }
  return n;
}

static tino_file_size_t
tino_data_file_posE(TINO_DATA *d)
{
  return tino_file_lseekE(d->user.fd, (tino_file_size_t)0, SEEK_CUR);
}

static tino_file_size_t
tino_data_file_seekE(TINO_DATA *d, tino_file_size_t pos)
{
  return tino_file_lseekE(d->user.fd, pos, SEEK_SET);
}

static tino_file_size_t
tino_data_file_seek_endE(TINO_DATA *d)
{
  return tino_file_lseekE(d->user.fd, (tino_file_size_t)0, SEEK_END);
}

static void
tino_data_file_closeA(TINO_DATA *d)
{
  if (tino_file_closeE(d->user.fd))
    tino_data_error(d, "file close error fd %d", d->user.fd);
}

struct tino_data_handler tino_data_file_handler	=
  {
    "tino_data file handle",
    tino_data_file_readI,
    tino_data_file_writeI,
    tino_data_file_syncA,
    tino_data_file_posE,
    tino_data_size_genericA,
    tino_data_file_seekE,
    tino_data_file_seek_endE,
    tino_data_file_closeA,
    0
  };

#define	TINO_DATA_FILE	(&tino_data_file_handler)

static TINO_DATA *
tino_data_fileA(TINO_DATA *d, int fd)
{
  if (fd<0)
    {
      tino_data_error(d, "file open error");
      return d;
    }
  return tino_data_handlerO(d, TINO_DATA_FILE, tino_data_user_fd(fd));
}


/**********************************************************************/

#if 0
/* THIS HERE SIMPLY DOES NOT WORK.
 *
 * Apparently, fread/fwrite do not work reliably and I do not know any
 * way to fix it except doing it unbuffered.  However then these
 * functions have no advantage over tino_data_file.  (It looks like
 * Stdlib does not survive EINTRs correctly.)
 *
 * Result: I have to create my own, reliable, IO functions which are
 * able to work with EINTR.
 */

/* It looks like stdlib is insane.  Period.
 *
 * I found no way to reset the error marker without loosing data.
 * Following function looses data in an unpredictable way:
 */
static int
tino_data_stream_io(FILE *fd, int n)
{
  int	e;

  e	= errno;
  if (ferror(fd))
    {
      clearerr(fd);	/* clear error and eof condition	*/
      if (!n)
	{
	  errno	= e;
	  return -1;
	}
    }
  else if (feof(fd))
    clearerr(fd);
  errno	= e;
  return n;
}

static int
tino_data_stream_read(TINO_DATA *d, void *ptr, size_t max)
{
  return tino_data_stream_io(d->user.file, fread(ptr, 1, max, d->user.file));
}

static int
tino_data_stream_write(TINO_DATA *d, const void *ptr, size_t max)
{
  return tino_data_stream_io(d->user.file, fwrite(ptr, 1, max, d->user.file));
}

static tino_file_size_t
tino_data_stream_pos(TINO_DATA *d)
{
  return tino_file_ftellE(d->user.file);
}

static tino_file_size_t
tino_data_stream_seek(TINO_DATA *d, tino_file_size_t pos)
{
  return tino_file_fseekE(d->user.file, pos, SEEK_SET) ? pos+1 : pos;
}

static tino_file_size_t
tino_data_stream_seek_end(TINO_DATA *d)
{
  return tino_file_fseekE(d->user.file, (tino_file_size_t)0, SEEK_END);
}

static void
tino_data_stream_close(TINO_DATA *d)
{
  if (tino_file_fcloseE(d->user.file))
    tino_data_error(d, "stream close error");
}

struct tino_data_handler tino_data_stream_handler	=
  {
    "tino_data file stream pointer",
    tino_data_stream_read,
    tino_data_stream_write,
    0,
    tino_data_stream_pos,
    tino_data_size_genericA,
    tino_data_stream_seek,
    tino_data_stream_seek_end,
    tino_data_stream_close,
    0
  };

#define	TINO_DATA_STREAM	(&tino_data_stream_handler)

static TINO_DATA *
tino_data_stream(TINO_DATA *d, FILE *fd)
{
  if (!fd)
    {
      tino_data_error(d, "filestream open error");
      return d;
    }
  return tino_data_handlerO(d, TINO_DATA_STREAM, tino_data_user_file(fd));
}
#endif

/**********************************************************************/

#undef cDP
#endif
