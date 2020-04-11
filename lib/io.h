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

#include "defs.h"
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
    int (*	put)(IOs, ...);			/* release handle from IO(fd)	*/
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
  loff_t	off;

  off	= lseek64(d->fd, (loff_t)pos, SEEK_SET);
  return off == pos ? 0 : -1;
}

static int
IOstdFlush(IOs d)
{
  return fdatasync(d->fd);
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

static int
IOput(IOs *io)
{
  va_list	list;
  int		ret;

  (*io)->use--;

  va_start(list, *io);
  ret	= va_arg(list, int);
  va_end(list);
  return ret;
}

static int
IOstdPut(IOs io, ...)
{
  return IOput(&io);
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
ioWriter(int fd, const void *ptr, int len)
{
  IOs	io = IO(fd);
  int	wd = alarmWatch(0);
  int	ret;

  ret	= io->write(io, ptr, len);
  io->put(io);
  alarmWatch(wd);
  return ret;
}

