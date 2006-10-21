/* $Header$
 *
 * Generic binding for (large) files.  To use this, you currently have
 * to include this as the *first* include before any other includes.
 *
 * If you want to use exceptions, then do:
 * #define TINO_FILE_EXCEPTIONS
 * #include "tino/file.h"	// don't include something before!
 * #include "tino/exception.h"	// don't include something between!
 * #include ...			// now include the rest
 *
 * Sorry, yes, this is broken design.  But it is all I have for now.
 * There will be a successor "tino/io.h" in some distant future which
 * will handle *all* IO the same way, regardless if it is a file,
 * directory, device, socket or telepathy (see below).  YKWIM.
 *
 * In case of exceptions, the most functions will only return in the
 * "good" or "reasonable" case.  Uncommon or error cases will raise an
 * exception (that's where exceptions are for).  Note that the old
 * behavior will vanish as soon as I consider exceptions to be stable
 * and mature.
 *
 * Until io.h is ready, you get this:
 *
 * There is far too many confusion out there about large file support.
 * There shall be no differences in the way files are handled, either
 * they are 1 byte long or 9^9^9^9^9^9^9^9^9^9^9^9^9^9 bytes.  YKWIM.
 *
 * This here shall be a generic large file support.  So if files hit
 * the 64 bit boundary (this will be arround 2040), this here shall
 * still be able to handle the biggest file ever found on your system.
 *
 * As long as I did not came accross doing it right, there is
 * following convention:
 *	tino_file_FUNC(const char *, ..)
 * has following companies:
 *	tino_file_FUNC_fd(int, ..)
 *	tino_file_fFUNC(FILE *, ..)
 * with some exceptions from this rule (tino_file_fopen for example).
 *
 * In some distant future I will start to implement my own "basic IO"
 * system (io.h) as there are too incompatible implementations which
 * cannot be fixed easily.  So you shall not assume that what's
 * returned by tino_file_open can be passed to read() or other
 * standard library functions.  However AFAICS the return value will
 * stay "near" the system's file descriptor (most times
 * tino_io_fd(io)==io), all you will miss then is some internal
 * bookkeeping (to work around all those broken systems out there),
 * however there will be some obfuscated sideeffects when
 * tino_io_fd(io)!=io!  If you don't want to adapt your programs
 * later, don't use this here yet, as there are missing a lot of
 * wrappers today.
 *
 * Also please note that the "companies" to functions then will start
 * to vanish, as all the cases then are handled all the same way.  I
 * prefer handles instead of pointers as pointers are fixed in memory
 * and are error prone.  It's a compiler lack that you cannot enforce
 * type checking on integers easily.  Also when 64 bit arrives, there
 * is plenty of room to add some "handle obfuscation" to integer
 * handles which are likely to never go over 16 bit.
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.24  2006-10-21 01:41:26  tino
 * chdir
 *
 * Revision 1.23  2006/07/26 11:58:25  tino
 * notsock() and notfile() added
 *
 * Revision 1.22  2006/07/22 23:47:44  tino
 * see ChangeLog (changes for mvatom)
 *
 * Revision 1.21  2006/07/22 17:27:23  tino
 * Three functions added
 *
 * Revision 1.20  2005/12/08 01:38:47  tino
 * forgot a return 0 in tino_file_close
 *
 * Revision 1.19  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.18  2005/09/04 23:10:51  tino
 * I shall try to compile before CI
 *
 * Revision 1.17  2005/09/04 23:04:57  tino
 * tino_file_read_line_x added
 *
 * Revision 1.16  2005/08/02 04:44:41  tino
 * C++ changes
 *
 * Revision 1.15  2005/06/28 20:10:28  tino
 * started to add IOW (IO wrapper)
 *
 * Revision 1.14  2005/04/25 23:07:01  tino
 * some new routines
 *
 * Revision 1.13  2005/04/24 13:44:11  tino
 * tino_file_notdir
 *
 * Revision 1.12  2005/04/24 12:55:38  tino
 * started GAT support and filetool added
 *
 * Revision 1.11  2005/03/05 19:42:54  tino
 * tino_file_mmap_anon added
 *
 * Revision 1.10  2005/01/25 22:14:51  tino
 * exception.h now passes include test (but is not usable).  See ChangeLog
 *
 * Revision 1.9  2004/10/16 21:48:56  tino
 * dev.h enabled, tino_trim added
 *
 * Revision 1.8  2004/10/10 12:44:37  tino
 * exception.h and file.h interaction updated
 *
 * Revision 1.7  2004/10/05 02:05:40  tino
 * A lot improvements, see ChangeLog
 *
 * Revision 1.6  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.5  2004/08/18 16:00:45  Administrator
 * AIO not available under CygWin
 *
 * Revision 1.4  2004/07/28 03:44:26  tino
 * Makefile changes
 *
 * Revision 1.3  2004/07/25 08:55:01  tino
 * initial prototype largefile support added
 *
 * Revision 1.2  2004/07/25 07:04:31  tino
 * *** empty log message ***
 *
 * Revision 1.1  2004/06/12 11:17:46  tino
 * removed too frequent printing of unneccessary warnings
 */

#ifndef tino_INC_file_h
#define tino_INC_file_h

#undef _GNU_SOURCE
#define _GNU_SOURCE

#ifdef _FEATURES_H
#error "file.h must be the very first include, even before stdio.h"
#endif

/* Yes, this is ugly, ugly, ugly ..
 */
#ifdef TINO_FILE_EXCEPTION
#define tino_INC_file_h_override
#include "exception.h"
#undef tino_INC_file_h_override
#endif

/* This is even more ugly, ugly, ugly ..
 * It should be defined above ..
 */

#include "type.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/mman.h>


/**********************************************************************/

typedef struct stat64	tino_file_stat_t;
typedef off64_t		tino_file_size_t;
typedef fpos64_t	tino_file_pos_t;


/**********************************************************************/

static int
tino_file_stat(const char *name, tino_file_stat_t *st)
{
  return stat64(name, st);
}

static int
tino_file_lstat(const char *name, tino_file_stat_t *st)
{
  return lstat64(name, st);
}

static int
tino_file_stat_fd(int fd, tino_file_stat_t *st)
{
  return fstat64(fd, st);
}

/* Returns 0 if DIR
 */
static int
tino_file_notdir(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_stat(name, &st))
    return -1;
  if (S_ISDIR(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_notexists(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstat(name, &st))
    return -1;
  return 0;
}

static int
tino_file_notsocket(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstat(name, &st))
    return -1;
  if (S_ISSOCK(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_notfile(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstat(name, &st))
    return -1;
  if (S_ISREG(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_mkdir(const char *dir)
{
  return mkdir(dir, 0755);
}

static int
tino_file_chdir(const char *dir)
{
  return chdir(dir);
}


/**********************************************************************/
/* Yes, there is no mode flag to open.  I hate the mode flag, as
 * applications usually really don't want to know anything about
 * modes, and it is error prone (in case you forget to give it).  If
 * you need mode, use tino_file_open_create or tino_file_create which
 * are more clear.
 *
 * Note that these here are not considered to operate on directories
 * nor softlinks (if not followed), only on files and similar types,
 * like terminals, sockets, devices, anything which gives data like a
 * file and which IO is portable across systems.  Perhaps in some
 * distant future I will hinder these to open dirs under Unix.
 */

static FILE *
tino_file_fopen(const char *name, const char *mode)
{
  return fopen64(name, mode);
}

static FILE *
tino_file_freopen(const char *name, const char *mode, FILE *fd)
{
  return freopen64(name, mode, fd);
}

#define tino_file_fdopen	tino_file_open_fd
static FILE *
tino_file_open_fd(int fd, const char *mode)
{
  return fdopen(fd, mode);
}

static int
tino_file_open(const char *name, int flags)
{
  return open64(name, flags&~(O_TRUNC|O_CREAT), 0664);
}

static int
tino_file_open_read(const char *name)
{
  return tino_file_open(name, O_RDONLY);
}

static int
tino_file_open_create(const char *name, int flags, int mode)
{
  return open64(name, (flags&~O_TRUNC)|O_CREAT, mode);
}

static int
tino_file_create(const char *name, int flags, int mode)
{
  return open64(name, flags|O_TRUNC|O_CREAT, mode);
}

static int
tino_file_close_intr(int fd)
{
  return close(fd);
}

static int
tino_file_close(int fd)
{
  while (tino_file_close_intr(fd))
    if (errno!=EINTR && errno!=EAGAIN)
      return -1;
  return 0;
}

/**********************************************************************/

static tino_file_size_t
tino_file_ftell(FILE *fd)
{
  return ftello64(fd);
}

static tino_file_size_t
tino_file_fseek(FILE *fd, tino_file_size_t pos, int whence)
{
  return fseeko64(fd, pos, whence);
}

static int
tino_file_fgetpos(FILE *fd, tino_file_pos_t *pos)
{
  return fgetpos64(fd, pos);
}

static int
tino_file_fsetpos(FILE *fd, const tino_file_pos_t *pos)
{
  return fsetpos64(fd, pos);
}


/**********************************************************************/

/* Truncate a file
 */
static int
tino_file_truncate(const char *name, tino_file_size_t size)
{
  return truncate64(name, size);
}

static int
tino_file_truncate_fd(int fd, tino_file_size_t size)
{
  return ftruncate64(fd, size);
}

/* Flush the data of a file.
 */
static int
tino_file_fflush(FILE *fd)
{
  return fflush(fd);
}

/* This does not sync the metadata nor the directory.
 *
 * This is usually what you want (if not, that's what I want):
 * Have a point of consistence of the contents of a file.
 */
static int
tino_file_flush_fd(int fd)
{
  return fdatasync(fd);
}

/* See tino_file_flush_fd
 * returns:
 * 0	success
 * <0	if open fails (return value of open)
 * 1	if tino_file_flush_fd fails
 * 2	if close fails (unlikely)
 */
static int
tino_file_flush(const char *name)
{
  int	fd;

  fd	= tino_file_open(name, O_RDONLY);
  if (fd<0)
    return fd;
  if (tino_file_flush_fd(fd))
    {
      close(fd);
      return 1;
    }
  if (close(fd))
    return 2;
  return 0;
}

/* Set a file to nonblocking
 * returns the previous blocking (0=noblock) or -1 on error
 */
static int
tino_file_blocking(int fd, int block)
{
  int	flag, new;

  flag	= fcntl(fd, F_GETFL);
  if (flag==-1)
    return -1;
  new	= block ? (flag&~O_NONBLOCK) : (flag|O_NONBLOCK);
  if (flag!=new && fcntl(fd, F_SETFL, (long)new))
    return -1;
  return flag&O_NONBLOCK;
}

static int
tino_file_nonblock(int fd)
{
  return tino_file_blocking(fd, 0);
}

static int
tino_file_block(int fd)
{
  return tino_file_blocking(fd, 1);
}


/**********************************************************************/
/* not ready */

/* Map an anonumous shared memory segment.
 * This is currenlty only thought for regions up to 1 GB.
 */
static void *
tino_file_mmap_anon(size_t len)
{
  int	fd;
  void	*tmp;

  if ((fd=open("/dev/zero", O_RDWR))==-1)
    return 0;

  tmp	= mmap64(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)0);
  close(fd);

  return tmp;
}

static void *
tino_file_mmap(void *adr, size_t len, int prot, int flag, int fd,
		tino_file_size_t size)
{
  return mmap64(adr, len, prot, flag, fd, size);
}

static int
tino_file_munmap(void *adr, size_t len)	/* like it symmetric */
{
  return munmap(adr, len);
}

/**********************************************************************/
/* AIO */

/* For some reason I get errors when including AIO with C++ */
#ifndef  __cplusplus
#ifndef __cygwin__
#include "file_aio.h"
#endif
#endif


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Extensions
 */

#define STAT2CMP(A,B,n)	(A).st_##n!=(B).st_##n
#define STATCMP(A,B)	(\
	   STAT2CMP(A,B,dev) \
	|| STAT2CMP(A,B,ino) \
	|| STAT2CMP(A,B,mode) \
	|| STAT2CMP(A,B,nlink) \
	|| STAT2CMP(A,B,uid) \
	|| STAT2CMP(A,B,gid) \
	|| STAT2CMP(A,B,rdev) \
	|| STAT2CMP(A,B,size) \
	|| STAT2CMP(A,B,blksize) \
	|| STAT2CMP(A,B,blocks) \
	|| STAT2CMP(A,B,atime) \
	|| STAT2CMP(A,B,mtime) \
	|| STAT2CMP(A,B,ctime) \
	)

/* Compare two STAT structures for equality
 * Perhaps we have to weed out some unwanted fields like atime,
 * we will see ..
 */
static int
tino_file_statcmp(const tino_file_stat_t *st1, const tino_file_stat_t *st2)
{
  return STATCMP(*st1, *st2);
}

/* Return if files are identical according to lstat()
 */
static int
tino_file_lstat_diff(const char *file1, const char *file2)
{
  tino_file_stat_t	st1, st2, st3, st4;
  int			i;

  if (tino_file_lstat(file1, &st1))
    return -1;
  if (tino_file_lstat(file2, &st2))
    return -2;
  if (st1.st_dev!=st2.st_dev ||
      st1.st_ino!=st2.st_ino)
    return 1;

  /* As some filesystems lie about the inode we cannot trust that alone.
   * Add some additional heuristics and test all fields.
   *
   * As we are in a multi user environment,
   * a file can change while we stat it.
   * I don't want to use locking in the first place,
   * so I retry the stat in a loop.  SIGH
   */
  for (i=1000; --i>0; )
    {
      if (tino_file_lstat(file1, &st3))
	return -1;
      if (tino_file_lstat(file2, &st4))
	return -2;
      if (!STATCMP(st1,st3) && !STATCMP(st2,st4))
	return STATCMP(st1,st2);
      st1	= st3;
      st2	= st4;
    }
  /* Well, this aparently did not work as a file changed too fast.
   * Now we have to use locking ..
   */
  000; /* XXX TODO XXX */
  errno=EAGAIN;
  return 2;
}


/**********************************************************************/
/* common wrappers
 *
 * PORTABILITY WARNING: What you read below is only correct for
 * systems, where EINTR can interrupt read/write only at the
 * beginning.  Systems which return -1 after partial transfers instead
 * of the short count are badly broken, but there are rumours that
 * such systems are still arround.  Sorry, they have to live with the
 * fact that some software shows unpredictable errors (as you cannot
 * seek back a pipe, what can you do?).
 *
 * Another thing to say: I hereby claim that read is allowed to clober
 * the parts of the buffer which do not hold data on return.  For
 * example I might want to fill buffers with 0 if not used, just to
 * wipe out traces of old information (like valuable passwords).  So
 * not-so-broken systems which give EINTR and clobber your buffer but
 * are able to wind back to where the read started are compatible to
 * these wrappers.  I don't know of any.
 *
 * A last note: Some of my software heavily depends on signals beeing
 * able to interrupt read/write which then must return partial
 * information.  There is no problem with Linux on that.
 */

/* This is usually not what you want.
 * Do a read which "fails" if interrupted by a signal.
 */
static int
tino_file_read_intr(int fd, char *buf, size_t len)
{
  if (len>SSIZE_MAX)
    len	= SSIZE_MAX;
  return read(fd, buf, len);
}

/* This is often what you want:
 * Read as much data from a file descriptor as you can,
 * and ignore interrupts as long as you don't have anything,
 * to make EOF detection more easy.
 *
 * On nonblocking IO you will get EAGAIN as usual.
 */
static int
tino_file_read(int fd, char *buf, size_t len)
{
  int	got;

  while ((got=tino_file_read_intr(fd, buf, len))<0 && errno==EINTR)
    {
      /* Now, there are systems where EINTR means death, as POSIX
       * allows to return -1 after data has been transferred.  *SIGH*
       *
       * We cannot detect this case after it occurred, so we have to
       * prepare against this case.  However this is slow and clumsy.
       * Leave this for the future in tino_io
       */
    }
  return got;
}

/* When you need something which is terminated by a CR or LF.
 * Note that this might read more than a line!
 */
static int
tino_file_read_line_x(int fd, char *buf, size_t len)
{
  int	got, have;

  for (have=0; have<len; have+=got)
    {
      int	i;

      got	= tino_file_read(fd, buf+have, len-have);
      if (got<=0)
        {
          if (have)
	    break;
          return got;
        }
      for (i=0; i<got; i++)
        if (buf[have+i]=='\r' || buf[have+i]=='\n')
          return have+got;
    }
  return have;
}

/* This is for lazy people who want easy going.
 * Usually you will use this when a short read means error
 * or you don't want to loop yourself.
 *
 * This returns the number of read bytes, where a short count means
 * error (including EAGAIN) or EOF.  You can see this examining errno.
 */
static int
tino_file_read_all(int fd, char *buf, size_t len)
{
  int	pos;

  for (pos=0; pos<len; )
    {
      int	got;

      errno	= 0;
      if ((got=tino_file_read_intr(fd, buf+pos, len-pos))>0)
	pos	+= got;
      else if (pos || errno!=EINTR)
	break;
    }
  return pos;
}

/* This is usually not what you want.
 * Do a write which "fails" if interrupted by a signal.
 */
static int
tino_file_write_intr(int fd, const char *buf, size_t len)
{
  return write(fd, buf, len);
}

/* This is often what you want:
 * Write as much data to a file descriptor as you can.
 * Ignore interrupts as long as you don't have written anything.
 *
 * On nonblocking IO you will get EAGAIN as usual.
 */
static int
tino_file_write(int fd, const char *buf, size_t len)
{
  int	got;

  while ((got=tino_file_write_intr(fd, buf, len))<0 && errno==EINTR)
    {
      /* Now, there are systems where EINTR means death, as POSIX
       * allows to return -1 after data has been transferred.  *SIGH*
       *
       * We cannot detect this case after it occurred, so we have to
       * prepare against this case.  However this is slow and clumsy.
       * Leave this for the future in tino_io
       */
    }
  return got;
}

/* This is for lazy people who want easy going.
 * Usually you will use this when a short write means error
 * or you don't want to loop yourself.
 *
 * This returns the number of written bytes, where a short count means
 * error (including EAGAIN) or EOF.  You can see this examining errno.
 */
static int
tino_file_write_all(int fd, const char *buf, size_t len)
{
  int	pos;

  for (pos=0; pos<len; )
    {
      int	put;

      errno	= 0;
      if ((put=tino_file_write_intr(fd, buf+pos, len-pos))>0)
	pos	+= put;
      /* Actually a 0 return of write is a little weird, as there is
       * nothing like an EOF here.  EOF means error, like broken pipe.
       * However we can think of it as an error case with errno set to
       * 0 (if not overwritten by the standard library).
       */
      else if (!put || errno!=EINTR)
	break;
    }
  return pos;
}


/**********************************************************************/

#endif
