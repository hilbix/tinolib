/* $Header$
 *
 * Generic IO layer.  For the moment this only includes tino/file.h
 * IF YOU WANT TO DEPEND ON THS, USE IT AS THE FIRST INCLUDE!
 *
 * In some distant future this will handle *all* IO the same way,
 * regardless if it is a file, directory, device, socket or telepathy.
 * YKWIM.
 *
 * Please read io.txt for more information.
 *
 * $Log$
 * Revision 1.2  2005-01-04 13:23:49  tino
 * see ChangeLog, mainly changes for "make test"
 *
 * Revision 1.1  2004/10/05 02:05:11  tino
 * added (prototypes)
 */

#ifndef tino_INC_io_h
#define tino_INC_io_h

#include "file.h"

/* Never even try to access this.
 * It will be renamed unconditionally!
 */
#define	I	tino_io_sdf923j2390s9234j

/* Note that 0 always is stdin, 1 always is stdout and 2 always is stderr
 * Even if you close it ;)
 */
static struct tino_io_glob
  {
    struct tino_io_imp
    int		fd;	/* -1: unused, -2:freed due to fd shortage	*/
    int		type;	/* internal type, 0 if not set	*/
  } *I;
static int	tino_io_fds;

static void
tino_io_init(int fds, void *ptr)
{
  I	= ptr ? ;
  000;
}

static int
tino_io_new(void)
{
  int	fd;

  /* Try to dup something
   */
  fd	= dup(2);
  if (fd<0)
    fd	= socket(PF_LOCAL, SOCK_DGRAM, 0);
  if (fd<0 || fd>0x10000000)
    tino_throw(TINO_EX_IO, "new");

  /* The fd is already taken internally,
   * hunt for a free slot ..
   */
  if (fd<tino_io_fds && I[fd].fd!=-1)
    {
      int	i;

      for (i=3; i<tino_io_fds; i++)
    }
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
    }
}

/* When can this happen?
 * When your system runs out of file handles.
 * tino_io is thought to be able to handle this case gracefully.
 * This can return -1 for error/not allocated,
 * or -2 if fd was closed, perhaps due to fd shortage.
 */
static int
tino_io_fd(int i)
{
  if (i<0 || i>=tino_io_fds)
    return -1;
  return I[i].fd;
}

int
tino_io_new(void)
{
}

#undef I

#endif
