/* $Header$
 *
 * Generic binding for (large) files.  To use this, you currently have
 * to include this as the *first* include before any other includes.
 *
 * There is far too many confusion out there about large file support.
 * There shall be no differences in the way files are handled, either
 * they are 1 byte long or 2^2222222222222222222222 bytes.  YKWIM.
 *
 * This here shall be a generic large file support.  So if files hit
 * the 64 bit boundary (this will be arround 2040), this here shall
 * still be able to handle the biggest file ever found on your system.
 *
 * Note that this might start to implement the own wrappers etc. when
 * I happen to hit incompatible implementations which cannot be fixed
 * easily.
 *
 * $Log$
 * Revision 1.2  2004-07-25 07:04:31  tino
 * *** empty log message ***
 *
 * Revision 1.1  2004/06/12 11:17:46  tino
 * removed too frequent printing of unneccessary warnings
 */

#ifndef tino_INC_file_h
#define tino_INC_file_h

#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <unistd.h>

/**********************************************************************/

typedef struct stat64	tino_file_stat;
typedef struct off64_t	tino_file_size;
typedef fpos64_t	tino_file_pos;
typedef struct aiocb64	tino_file_aio;

/**********************************************************************/

static int
tino_file_stat(const char *name, tino_file_stat *st)
{
  return stat64(name, st);
}

static int
tino_file_lstat(const char *name, tino_file_stat *st)
{
  return lstat64(name, st);
}

static int
tino_file_fstat(int fd, tino_file_stat *st)
{
  return fstat64(fd, st);
}

/**********************************************************************/
/* Yes, there is no mode.  I hate the mode flag,
 * as applications usually really don't want to know anything about modes.
 * It only hinders clean implementations.
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

static FILE *
tino_file_fdopen(int fd, const char *mode)
{
  return fdopen64(name, mode);
}

static int
tino_file_open(const char *name, int mode)
{
  return open64(name, mode, 0775);
}

/**********************************************************************/

static tino_file_size
tino_file_ftell(FILE *fd)
{
  return ftello64(fd);
}

static tino_file_size
tino_file_fseek(FILE *fd, tino_file_size pos, int whence)
{
  return fseeko64(fd, pos, whence);
}

static int
tino_file_fgetpos(FILE *fd, tino_file_pos *pos)
{
  return fgetpos64(fd, pos);
}

static int
tino_file_fsetpos(FILE *fd, const tino_file_pos *pos)
{
  return fsetpos64(fd, pos);
}

/**********************************************************************/

static int
tino_file_truncate(const char *name, tino_file_size size)
{
  return truncate64(name, size);
}

static int
tino_file_ftruncate(int fd, tino_file_size size)
{
  return ftruncate64(fd, size);
}

/**********************************************************************/

static int
tino_file_mmap(void *adr, size_t len, int prot, int flag, int fd,
		tino_file_size size)
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

static int
tino_file_aread(tino_file_aio *cbp)
{
  return aio_read64(cbp);
}

static int
tino_file_awrite(tino_file_aio *cbp)
{
  return aio_write64(cbp);
}

static int
tino_file_listio(int mode, tino_file_aio * const list[], int n,
		  struct sigevent *sig)
{
  return lio_listio64(mode, list, n, sig);
}

static int
tino_file_aerror(const tino_file_aio *cbp)
{
  return aio_error64(cbp);
}

static int
tino_file_areturn(const tino_file_aio *cbp)
{
  return aio_return64(cbp);
}

static int
tino_file_async(int op, tino_file_aio *cbp)
{
  return aio_fsync64(cbp);
}

static int
tino_file_suspend(int op, tino_file_aio *cbp)
{
  return aio_fsync64(cbp);
}


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
tino_file_statcmp(const tino_file_stat *st1, const tino_file_stat *st2)
{
  return STATCMP(*st1, *st2);
}

/* Return if files are identical according to lstat()
 */
static int
tino_file_lstat_diff(const char *file1, const char *file2)
{
  tino_file_stat	st1, st2, st3, st4;
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

#endif
