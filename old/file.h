/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-06-12 11:17:46  tino
 * removed too frequent printing of unneccessary warnings
 *
 */

#ifndef tino_INC_file_h
#define tino_INC_file_h

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
tino_file_statcmp(const struct stat *st1, const struct stat *st2)
{
  return STATCMP(*st1, *st2);
}


/* Return if files are identical according to lstat()
 */
static int
tino_file_lstat_diff(const char *file1, const char *file2)
{
  struct stat	st1, st2, st3, st4;
  int		i;

  if (lstat(file1, &st1))
    return -1;
  if (lstat(file2, &st2))
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
      if (lstat(file1, &st3))
	return -1;
      if (lstat(file2, &st4))
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
