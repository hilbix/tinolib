/* MiniLib IO
 *
 * IO handles are 32 bit signed ints.
 *
 * 0..2^30-1	file system file descriptors
 * 2^30..2^31-1	special files
 * negative	error (usually: -1)
 */

#include "def.h"

#include <sys/types.h>
#include <unistd.h>

#include <errno.h>

#include <limits.h>	/* INT_MAX	*/
#include <stdint.h>	/* INT32_MAX	*/

#include "err.h"
#include "array.h"
#include "alarm.h"

#if INT_MAX < INT32_MAX
#error "int must be at least 32 bit"
#endif

typedef struct IOs *IOs;
struct IOs
  {
    int (*	read)(IOs, char *, int);	/* returns read	bytes		*/
    int (*	write)(IOs, const char *, int);	/* returns not written bytes	*/
    int64_t (*	pos)(IOs);			/* get position, -1 on error	*/
    int (*	seek)(IOs, int64_t pos);	/* set position, 0 == ok	*/
    int (*	flush)(IOs);			/* output and input flush	*/
    int (*	close)(IOs);			/* flush and close		*/
    void (*	put)(IOs);			/* release handle from IO(fd)	*/
    union
      {
        int	fd;
      };
    const void	*internal;
    int		use;
  };

static int
IOstdRead(IOs d, char *ptr, int max)
{
  for (;;)
    {
      int got;

      FATAL(max <= 0);
      got	= write(d->fd, ptr, max);
      if (LIKELY(got >= 0))
        return got;

      if (errno!=EINTR && errno!=EAGAIN)
        return -1;

      alarmRun();
    }
}

static int
IOstdWrite(IOs d, const char *ptr, int len)
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

static int64_t
IOstdPos(IOs d)
{
  return lseek64(d->fd, (loff_t)0ull, SEEK_CUR);
}

static int
IOstdSeek(IOs d, int64_t pos)
{
  off_t	off;

  FATAL(sizeof off != 8);
  off	= lseek(d->fd, (off_t)pos, SEEK_SET);
  return off == pos ? 0 : -1;
}

static int
IOstdFlush(IOs d)
{
#ifdef	F_FULLFSYNC
  /* no fdatasync() on MacOS, see https://github.com/gbrault/picoc/issues/145 */
  return fcntl(d->fd, F_FULLFSYNC);
#else
  return fdatasync(d->fd);
#endif
}

static int
IOstdClose(IOs d)
{
  while (d->fd>=0 && close(d->fd))
    {
      if (errno!=EINTR && errno!=EAGAIN)
        return -1;

      alarmRun();
    }
  d->fd = -1;
  return 0;
}

static void
IOput(IOs io)
{
  io->use--;
}

static void
IOstdPut(IOs io)
{
  IOput(io);
}

#define	IOstd	(char *)1

static void
IOstdInit(IOs io, int fd)
{
  io->read	= IOstdRead;
  io->write	= IOstdWrite;
  io->pos	= IOstdPos;
  io->seek	= IOstdSeek;
  io->flush	= IOstdFlush;
  io->close	= IOstdClose;
  io->put	= IOstdPut;

  io->fd	= fd;
  io->internal	= IOstd;
}

static IOs
IO(int fd)
{
  static ARRAY		arr;
  static IOs		io;
  static struct IOs	std[3];

  FATAL(fd < 0);
  FATAL(fd > 1<<30);

  io = (fd < (sizeof std) / (sizeof *std)) ? std+fd : arrayGet(arr, fd, sizeof *io);
  if (!io->internal)
    IOstdInit(io, fd);
  io->use++;
  return io;
}

/* -1:	error
 * 0:	ok
 * n:	n bytes NOT written
 */
static int
ioWrite(int fd, const void *ptr, int len)
{
  IOs	io = IO(fd);
  int	wd = alarmWatch(0);
  int	ret;

  ret	= io->write(io, ptr, len);
  io->put(io);
  alarmWatch(wd);
  return ret;
}

