/* Generic binding for (large) files.  To use this, you currently have
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

#include "sysfix.h"
#include "type.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/mman.h>

#ifndef	TINO_XXX
#define	TINO_XXX
#endif


/**********************************************************************/

typedef TINO_T_stat	tino_file_stat_t;
typedef TINO_T_off_t	tino_file_size_t;
typedef TINO_T_fpos_t	tino_file_pos_t;


/**********************************************************************/

static int
tino_file_statE(const char *name, tino_file_stat_t *st)
{
  return TINO_F_stat(name, st);
}

static int
tino_file_lstatE(const char *name, tino_file_stat_t *st)
{
  return TINO_F_lstat(name, st);
}

static int
tino_file_stat_fdE(int fd, tino_file_stat_t *st)
{
  return TINO_F_fstat(fd, st);
}

/* Returns 0 if DIR
 */
static int
tino_file_notdirE(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_statE(name, &st))
    return -1;
  if (S_ISDIR(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_notexistsE(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstatE(name, &st))
    return -1;
  return 0;
}

static int
tino_file_notsocketE(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstatE(name, &st))
    return -1;
  if (S_ISSOCK(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_notfileE(const char *name)
{
  tino_file_stat_t	st;

  if (tino_file_lstatE(name, &st))
    return -1;
  if (S_ISREG(st.st_mode))
    return 0;
  return 1;
}

static int
tino_file_mkdirE(const char *dir)
{
  return TINO_F_mkdir(dir, 0755);
}

static int
tino_file_chdirE(const char *dir)
{
  return TINO_F_chdir(dir);
}

static int
tino_file_rmdirE(const char *dir)
{
  return TINO_F_rmdir(dir);
}

static int
tino_file_unlinkE(const char *file)
{
  return TINO_F_unlink(file);
}

static void
tino_file_unlinkO(const char *name)
{
  int	e;

  e	= errno;
  tino_file_unlinkE(name);
  errno	= e;
}

/* Rename with unlink according to POSIX.
 * DO NOT USE THIS, EVER!
 *
 * Unlinking a file unconditionally is a extremely bad sideffect.
 * Under Linux this has been fixed with renameat2-syscall.
 * However this is not supported on platforms except Linux.
 */
static int
tino_file_rename_unlinkEbs(const char *old, const char *newname)
{
  return rename(old, newname);	/* cannot EINTR	*/
}

/* Rename without unlink race condition
 */
static int
tino_file_renameE(const char *old, const char *newname)
{
#if ! defined(TINO_NO_renameat2)
  return renameat2(AT_FDCWD, old, AT_FDCWD, newname, RENAME_NOREPLACE);
#elif defined(SYS_renameat2)
  return syscall(SYS_renameat2, AT_FDCWD, old, AT_FDCWD, newname, (unsigned)(RENAME_NOREPLACE));
#else
  000;	/* have to implement this on unsupported platforms	*/
  errno = ENOTSUP;
  return -1;
#endif
}

#ifdef NOT_READY
/* Do a rename which does not overwrite.  Hardlink with unlink.
 *
 * This assumes that the source is not changed while it is linked
 * (therefor the sideffect warning).  For a better routine see
 * tino_file_rename_with_helperEs.
 *
 * This needs two syscalls:
 *
 * Hardlink and then remove the old name.  This may leave a hardlink
 * if interrupted at the wrong place, but sorry, I cannot help for
 * that.  Blame POSIX for forgetting an atomic rename_without_unlink
 */
static int
tino_file_rename_onlyEs(const char *old, const char *new)
{
  int	err;

  err	= link(old, new);
  000;
  if (err)
    return err;
}
#endif

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
tino_file_fopenE(const char *name, const char *mode)
{
  return TINO_F_fopen(name, mode);
}

static FILE *
tino_file_freopenE(const char *name, const char *mode, FILE *fd)
{
  return TINO_F_freopen(name, mode, fd);
}

#define tino_file_fdopenE	tino_file_open_fdE
static FILE *
tino_file_open_fdE(int fd, const char *mode)
{
  return TINO_F_fdopen(fd, mode);
}

static int
tino_file_openI(const char *name, int flags)
{
#ifdef O_LARGEFILE
  flags	|= O_LARGEFILE;
#endif
  return TINO_F_open(name, flags&~(O_TRUNC|O_CREAT), 0664);
}

static int
tino_file_openE(const char *name, int flags)
{
  for (;;)
    {
      int	fd;

      fd	= tino_file_openI(name, flags);
      if (fd>=0 || errno!=EINTR)
	return fd;
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
}

static int
tino_file_open_readE(const char *name)
{
  return tino_file_openE(name, O_RDONLY);
}

static int
tino_file_open_rwE(const char *name)
{
  return tino_file_openE(name, O_RDWR);
}

static int
tino_file_open_createI(const char *name, int flags, int mode)
{
#ifdef O_LARGEFILE
  flags	|= O_LARGEFILE;
#endif
  return TINO_F_open(name, (flags&~O_TRUNC)|O_CREAT, mode);
}

static int
tino_file_open_createE(const char *name, int flags, int mode)
{
  for (;;)
    {
      int	fd;

      fd	= tino_file_open_createI(name, flags, mode);
      if (fd>=0 || errno!=EINTR)
	return fd;
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
}

static int
tino_file_create_truncateE(const char *name, int flags, int mode)
{
#ifdef O_LARGEFILE
  flags	|= O_LARGEFILE;
#endif
  return TINO_F_open(name, flags|O_TRUNC|O_CREAT, mode);
}

/** Close file descriptor, may and return EINTR
 */
static int
tino_file_closeI(int fd)
{
  return TINO_F_close(fd);
}

/** Close file descriptor
 */
static int
tino_file_closeE(int fd)
{
  while (tino_file_closeI(fd))
    {
      if (errno!=EINTR)
	return -1;
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
  return 0;
}

/** Close file descriptor without sideffects on errno
 */
static void
tino_file_close_ignO(int fd)
{
  int	e;

  e	= errno;
  tino_file_closeE(fd);
  errno	= e;
}

/** Open /dev/null
 */
static int
tino_file_nullE(void)
{
  return tino_file_open_rwE("/dev/null");
}


/**********************************************************************/

static char *
tino_file_fgetsE(FILE *fd, char *ptr, size_t len)	/* including \n	*/
{
  return TINO_F_fgets(ptr, len-1, fd);	/* the -1 is to compensate for longstanding Borland bugs	*/
}

static int
tino_file_freadE(FILE *fd, void *ptr, size_t len)
{
  return TINO_F_fread(ptr, (size_t)1, len, fd);
}

static int
tino_file_fwriteE(FILE *fd, void *ptr, size_t len)
{
  return TINO_F_fwrite(ptr, (size_t)1, len, fd);
}

static int
tino_file_fcloseE(FILE *fd)
{
  return TINO_F_fclose(fd);
}

static int
tino_file_ferrorO(FILE *fd)
{
  return TINO_F_ferror(fd);
}

static int
tino_file_feofO(FILE *fd)
{
  return TINO_F_feof(fd);
}

static void
tino_file_clearerr(FILE *fd)
{
  int	e;

  e	= errno;
  TINO_F_clearerr(fd);
  errno	= e;
}


/**********************************************************************/

static tino_file_size_t
tino_file_lseekE(int fd, tino_file_size_t pos, int whence)
{
  return TINO_F_lseek(fd, pos, whence);
}

static tino_file_size_t
tino_file_ftellE(FILE *fd)
{
  return TINO_F_ftello(fd);
}

static tino_file_size_t
tino_file_fseekE(FILE *fd, tino_file_size_t pos, int whence)
{
  return TINO_F_fseeko(fd, pos, whence);
}

static int
tino_file_fgetposE(FILE *fd, tino_file_pos_t *pos)
{
  return TINO_F_fgetpos(fd, pos);
}

static int
tino_file_fsetposE(FILE *fd, const tino_file_pos_t *pos)
{
  return TINO_F_fsetpos(fd, pos);
}

static void
tino_file_dup2E(int fd_old, int fd_new)
{
  TINO_F_dup2(fd_old, fd_new);
}

/* According to manual, pipe() never returns EINTR
 */
static int
tino_file_pipeE(int fds[2])
{
  return TINO_F_pipe(fds);
}


/**********************************************************************/

/* Truncate a file
 */
static int
tino_file_truncateE(const char *name, tino_file_size_t size)
{
  return TINO_F_truncate(name, size);
}

static int
tino_file_truncate_fdE(int fd, tino_file_size_t size)
{
  return TINO_F_ftruncate(fd, size);
}

/* Flush the data of a file.
 */
static int
tino_file_fflushE(FILE *fd)
{
  return TINO_F_fflush(fd);
}

/* This does not sync the metadata nor the directory.
 *
 * This is usually what you want (if not, that's what I want):
 * Have a point of consistence of the contents of a file.
 */
static int
tino_file_flush_fdE(int fd)
{
  return TINO_F_fdatasync(fd);
}

/* See tino_file_flush_fd
 * returns:
 * 0	success
 * <0	if open fails (return value of open)
 * 1	if tino_file_flush_fd fails
 * 2	if close fails (unlikely)
 */
static int
tino_file_flushE(const char *name)
{
  int	fd;

  fd	= tino_file_openE(name, O_RDONLY);
  if (fd<0)
    return fd;
  if (tino_file_flush_fdE(fd))
    {
      close(fd);
      return 1;
    }
  if (tino_file_closeE(fd))
    return 2;
  return 0;
}

/* Set a file to nonblocking
 * returns the previous blocking (0=noblock) or -1 on error
 */
static int
tino_file_blockingE(int fd, int block)
{
  int	flag, upd;

  flag	= TINO_F_fcntl(fd, F_GETFL);
  if (flag==-1)
    return -1;
  upd	= block ? (flag&~O_NONBLOCK) : (flag|O_NONBLOCK);
  if (flag!=upd && TINO_F_fcntl(fd, F_SETFL, (long)upd))
    return -1;
  return flag&O_NONBLOCK;
}

static int
tino_file_nonblockE(int fd)
{
  return tino_file_blockingE(fd, 0);
}

static int
tino_file_blockE(int fd)
{
  return tino_file_blockingE(fd, 1);
}

/** Place a lock on the file (at the beginning).
 *
 * It can be an read or write lock, and the lock call can be blocking
 * (waiting for the lock to be possible) or not.
 */
static int
tino_file_lockI(int fd, int writelock, int block)
{
  struct flock	lk;

  lk.l_type	= writelock ? F_WRLCK : F_RDLCK;
  lk.l_whence	= 0;
  lk.l_start	= 0;
  lk.l_len	= 0;
  return TINO_F_fcntl(fd, block ? F_SETLKW : F_SETLK, &lk);
}

/** Remove a lock on the file (at the beginning).
 */
static int
tino_file_unlockI(int fd)
{
  struct flock	lk;

  lk.l_type	= F_UNLCK;
  lk.l_whence	= 0;
  lk.l_start	= 0;
  lk.l_len	= 0;
  return TINO_F_fcntl(fd, F_SETLK, &lk);
}

/** Like before, but handles EINTR
 *
 * Returns:
 * 0 if lock was successful
 * 1 if lock was not successful (block==0)
 * -1 on error
 */
static int
tino_file_lockE(int fd, int writelock, int block)
{
  while (tino_file_lockI(fd, writelock, block))
    {
      if (!block && (errno==EACCES || errno==EAGAIN))
	return 1;
      if (errno!=EINTR)
	return -1;
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
  return 0;
}

/** Like before, but handles EINTR
 *
 * Returns:
 * 0 if unlock was successful
 * -1 on error
 */
static int
tino_file_unlockE(int fd)
{
  while (tino_file_unlockI(fd))
    {
      if (errno!=EINTR)
	return -1;
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    }
  return 0;
}

/** Convenience routine
 */
static int
tino_file_lock_exclusiveE(int fd, int block)
{
  return tino_file_lockE(fd, 1, block);
}

/** Convenience routine
 */
static int
tino_file_lock_sharedE(int fd, int block)
{
  return tino_file_lockE(fd, 0, block);
}

/** Set close on exec flag:
 * =0: no close on exec
 * >0: close on exec
 * <0: do not change (just return current state)
 *
 * returns:
 * <0: error
 * =0: no close-on-exec
 * >0: close-on-exec present
 */
static int
tino_file_close_on_execE(int fd, int close_on_exec)
{
  int	flag, upd;

  flag	= TINO_F_fcntl(fd, F_GETFD);
  if (flag==-1)
    return -1;
  if (close_on_exec>=0)
    {
      upd	= close_on_exec ? (flag|O_CLOEXEC) : (flag&~O_CLOEXEC);
      if (flag!=upd && TINO_F_fcntl(fd, F_SETFD, upd))
        return -1;
    }
  return flag&O_CLOEXEC;
}

static int
tino_file_close_on_exec_getE(int fd)
{
  return tino_file_close_on_execE(fd, -1);
}

static int
tino_file_close_on_exec_setE(int fd)
{
  return tino_file_close_on_execE(fd, 1);
}

static int
tino_file_no_close_on_execE(int fd)
{
  return tino_file_close_on_execE(fd, 0);
}


/**********************************************************************/
/* not ready */

/* Map an anonumous shared memory segment.
 * This is currenlty only thought for regions up to 1 GB.
 */
static void *
tino_file_mmap_anonE(size_t len)
{
  int	fd;
  void	*tmp;

  if ((fd=TINO_F_open("/dev/zero", O_RDWR))==-1)
    return 0;

  tmp	= TINO_F_mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)0);
  close(fd);

  return tmp;
}

static void *
tino_file_mmapE(void *adr, size_t len, int prot, int flag, int fd,
		tino_file_size_t offset)
{
  return TINO_F_mmap(adr, len, prot, flag, fd, offset);
}

static int
tino_file_munmapE(void *adr, size_t len)	/* like it symmetric */
{
  return TINO_F_munmap(adr, len);
}

/**********************************************************************/
/* AIO */

#ifndef	TINO_NO_INCLUDE_AIO
#include "file_aio.h"
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
tino_file_statcmpO(const tino_file_stat_t *st1, const tino_file_stat_t *st2)
{
  return STATCMP(*st1, *st2);
}

/* Return if files are identical according to lstat()
 */
static int
tino_file_lstat_diffE(const char *file1, const char *file2)
{
  tino_file_stat_t	st1, st2, st3, st4;
  int			i;

  if (tino_file_lstatE(file1, &st1))
    return -1;
  if (tino_file_lstatE(file2, &st2))
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
      if (tino_file_lstatE(file1, &st3))
	return -1;
      if (tino_file_lstatE(file2, &st4))
	return -2;
      if (!STATCMP(st1,st3) && !STATCMP(st2,st4))
	return STATCMP(st1,st2);
      st1	= st3;
      st2	= st4;
    }
  /* Well, this aparently did not work as a file changed too fast.
   * Now we have to use locking ..
   */
  TINO_XXX;
  errno=EAGAIN;
  return 2;
}


/**********************************************************************/
/* Also see helpers.h (helpers.h shall no more be used!)	*/
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
tino_file_readI(int fd, void *buf, size_t len)
{
  if (len>SSIZE_MAX)
    len	= SSIZE_MAX;
  return TINO_F_read(fd, buf, len);
}

/* This is often what you want:
 * Read as much data from a file descriptor as you can,
 * and ignore interrupts as long as you don't have anything,
 * to make EOF detection more easy.
 *
 * On nonblocking IO you will get EAGAIN as usual.
 */
static int
tino_file_readE(int fd, char *buf, size_t len)
{
  int	got;

  do
    {
      got	= tino_file_readI(fd, buf, len);
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
      /* Now, there are systems where EINTR means death, as POSIX
       * allows to return -1 after data has been transferred.  *SIGH*
       *
       * We cannot detect this case after it occurred, so we have to
       * prepare against this case.  However this is slow and clumsy.
       * Leave this for the future in tino_io
       */
    } while (got<0 && errno==EINTR);
  return got;
}

/* When you need something which is terminated by a CR or LF.
 * Note that this might read more than a line!
 */
static int
tino_file_read_line_xE(int fd, char *buf, size_t len)
{
  size_t	have;
  int		got;

  for (have=0; have<len; have+=got)
    {
      int	i;

      got	= tino_file_readE(fd, buf+have, len-have);
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
tino_file_read_allE(int fd, void *_buf, size_t len)
{
  size_t	pos;
  char		*buf=(char *)_buf;

  for (pos=0; pos<len; )
    {
      int	got;

      errno	= 0;
      if ((got=tino_file_readE(fd, buf+pos, len-pos))>0)
	pos	+= got;
      else if (!got || pos)
	break;
      else
	return -1;
    }
  return pos;
}

/* This is usually not what you want.
 * Do a write which "fails" if interrupted by a signal.
 */
static int
tino_file_writeI(int fd, const void *buf, size_t len)
{
  return TINO_F_write(fd, buf, len);
}

/* This is often what you want:
 * Write as much data to a file descriptor as you can.
 * Ignore interrupts as long as you don't have written anything.
 *
 * On nonblocking IO you will get EAGAIN as usual.
 */
static int
tino_file_writeE(int fd, const void *buf, size_t len)
{
  int	got;

  do
    {
      got	= tino_file_writeI(fd, buf, len);

      /* Now, there are systems where EINTR means death, as POSIX
       * allows to return -1 after data has been transferred.  *SIGH*
       *
       * We cannot detect this case after it occurred, so we have to
       * prepare against this case.  However this is slow and clumsy.
       * Leave this for the future in tino_io
       */
#ifdef TINO_ALARM_RUN
      TINO_ALARM_RUN();
#endif
    } while (got<0 && errno==EINTR);

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
tino_file_write_allE(int fd, const void *buf, size_t len)
{
  size_t	pos;

  for (pos=0; pos<len; )
    {
      int	put;

      errno	= 0;
      if ((put=tino_file_writeE(fd, ((const char *)buf)+pos, len-pos))<=0)
	break;
      pos	+= put;

      /* Actually a 0 return of write is a little weird, as there is
       * nothing like an EOF here.  EOF means error, like broken pipe.
       * However we can think of it as an error case with errno set to
       * 0 (if not overwritten by the standard library).
       */
    }
  if (errno==EINTR)
    errno = 0;
  return pos;
}

/* Copy from in to out
 *
 * Returns 0 on EOF, <0 on read error, >0 on write error
 */
static int
tino_file_fd_copyE(int in, int out)
{
  char	block[BUFSIZ];
  int	got;

  while ((got=tino_file_readE(in, block, sizeof block))>0 &&
	 tino_file_write_allE(out, block, got)==got);
  return got;
}

/**********************************************************************/

#endif
