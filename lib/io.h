/* MiniLib IO
 *
 * IO handles are 32 bit signed ints.
 *
 * 0..2^30-1	file system file descriptors
 * 2^30..2^31-1	special files
 * negative	error (usually: -1)
 */

#include <errno.h>
#include <unistd.h>

#include <limits.h>	/* INT_MAX	*/
#include <stdint.h>	/* INT32_MAX	*/

#include "defs.h"
#include "err.h"
#include "array.h"
#include "alarm.h"

#if INT_MAX < INT32_MAX
#error "int must be at least 32 bit"
#endif

typedef struct IOs IOs;
struct IOs
  {
    int (*	read)(IOs *, char *, int);		/* returns read	bytes		*/
    int (*	write)(IOs *, const char *, int);	/* returns not written bytes	*/
    int64_t (*	get)(IOs *);				/* get position			*/
    int (*	set)(IOs *, int64_t pos);		/* set position			*/
    int (*	flush)(IOs *);				/* output and input flush	*/
    int (*	close)(IOs *);				/* flush and close		*/
    int (*	put)(IOs *, int);			/* release handle from IO(fd)	*/
    union
      {
        int	fd;
      };
    const void	*internal;
    int		use;
  };

static int
ioWriteFd(IOs *d, const char *ptr, int len)
{
  while (len)
    {
      int put;

      FATAL(len <= 0);
      put	= write(d->fd, ptr, len);
      if (LIKELY(put > 0))
        {
          ptr += put;
          len -= put;
          continue;
        }
      if (LIKELY(!put))
        break;

      if (errno!=EINTR && errno!=EAGAIN)
        return -1;

      alarmRun();
    }
  return len;
}

static IOs *
IO(int fd)
{
  static ARRAY	arr;
  static IOs	std[3], *io;

  FATAL(fd < 0);
  FATAL(fd > 1<<30);

  io = (fd < (sizeof std) / (sizeof *std)) ? std+fd : arrayGet(arr, fd, sizeof *io);
  if (!io->internal)
    IOstdInit(io, fd);
  return io;
}

/* -1:	error
 * 0:	ok
 * n:	n bytes NOT written
 */
static int
ioWriter(int fd, const void *ptr, int len)
{
  IOs	*io = IO(fd);
  int	wd = alarmWatch(0);
  int	ret;

  ret	= io->write(io, ptr, len);
  io->put(io, 0);
  alarmWatch(wd);
  return ret;
}

