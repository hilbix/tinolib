/* $Header$
 *
 * Reliable SBRK handling.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 *
 * $Log$
 * Revision 1.2  2007-09-26 21:29:46  tino
 * make test works again
 *
 * Revision 1.1  2007/09/26 13:26:28  tino
 * sbrk() varies.  So there now is a reliable wrapper.
 *
 */

#ifndef tino_INC_sbrk_h
#define tino_INC_sbrk_h

#include <unistd.h>
#include <errno.h>

/* Let all the various sbrk() implementations reliably return 0 on OOM
 * situations
 */
static void *
tino_sbrk(int mem)
{
  void	*p;

  p	= sbrk(mem);
  if (p && p!=(void *)-1 && (!mem || p!=sbrk(0)))
    return p;
  errno	= ENOMEM;
  return 0;
}

#endif
