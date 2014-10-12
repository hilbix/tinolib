/* Functions dealing with runtime privileges (not passwd nor PAM)
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_privilege_h
#define tino_INC_privilege_h

#include <unistd.h>

/* This ends the escalated privilege.  It shall also give up saved
 * elevation.
 *
 * Returns bit 1 for setregid() was done, bit 2 for setreuid() was
 * done or -1 on error.
 *
 * I REALLY HATE THIS!  There is a call setresuid() to really drop all
 * privileges.  However it is nonstandard.  So I really only hope that
 * the call setreuid() can drop all privileges, too, on all systems.
 */
int
tino_privilege_end_elevationE(void)
{
  int	ret;
  int	tmp;

  ret	= 0;
  if (getegid()!=(tmp=getgid()))
    {
      ret	|= 2;
      if (setregid(tmp, tmp))
	ret	= -1;	/* drop as much as possible!	*/
    }
  if (geteuid()!=(tmp=getuid()))
    {
      ret	|= 1;
      if (setreuid(tmp, tmp))
	ret	= -1;
    }
  return ret;
}

#endif
