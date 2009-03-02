/* $Header$
 *
 * Check (nonblocking) if there is some input waiting.
 * For a TTY this means, a key is pressed or a line was input.
 *
 * This unit probably is badly named.
 * However such bad names can be remembered more easily.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 *
 * $Log$
 * Revision 1.2  2009-03-02 15:55:11  tino
 * Timeout added
 *
 * Revision 1.1  2008-05-30 16:58:33  tino
 * Added keypressed.h
 */
#ifndef tino_INC_keypressed_h
#define tino_INC_keypressed_h

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <errno.h>

/* Nonblocking check if input data is available on the given FD
 *
 * This function is covered by the CLL!
 *
 * Returns 0 if there is data waiting
 * Returns 1 if there is no data available
 * Returns -1 on error (including EINTR)
 *
 * Bugs:
 * - Result is undefined on unsupported types
 * - Supported types are TTYs and socket()s
 * - Often it sworks on Pipes, too.
 * - It is not thought to work on files.
 */
static int
tino_nokeypressedI(int fd, unsigned long timeout)
{
  fd_set		fds;
  int			count;
  struct timeval	tv;

  /* First check via ioctl(), this is supported on TTYs.
   *
   * Often socket()s and sometimes pipe()s support this, too.
   */
  if (ioctl(0, FIONREAD, &count)>=0 && count>0)
    return 0;

  /* Now try with select().
   *
   * This is known to work for socket()s and most time TTYs.
   * Sometimes this works for pipe()s.
   * Often the return call is sane for files, too.
   */
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  tv.tv_sec	= timeout/1000;
  tv.tv_usec	= (timeout%1000)*1000l;
  errno		= 0;
  switch (select(fd+1, &fds, NULL, NULL, &tv))
    {
    case 0:	/* There is no bit set in fds, so we have no read condition	*/
      return 1;
    case 1:	/* There is a bit set in fds, can only be the bit of the FD	*/
      return 0;

    default:	/* Weird thing happened	*/
      return -1;
    }
}

/* As before, but handle EINTR gracefully (by retry)
 */
static int
tino_nokeypressedE(int fd, unsigned long timeout)
{
  int ret;

  while ((ret=tino_nokeypressedI(fd, timeout))<0 && errno==EINTR);
  return ret;
}

#ifdef TINO_TEST_MAIN
#include <stdlib.h>

/* Returns 0 (true) if there is input data waiting.
 * Returns 1 if no input data waiting.
 * Returns 255 on errors.
 * Return undefined on unknown input types.
 */
int
main(int argc, char **argv)
{
  return tino_nokeypressedE(0, (argc==2 ? strtoul(argv[1], NULL, 0l) : 0ul));
}
#endif
#endif
