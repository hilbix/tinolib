/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-10-10 12:07:14  tino
 * added
 *
 */

#ifndef tino_INC_privilege_h
#define tino_INC_privilege_h

#include <unistd.h>

/* This ends the escalated privilege.  It shall also gives up saved
 * elevation.
 *
 * I REALLY HATE THIS!  There is a call setresuid() to really drop all
 * privileges.  However it is nonstandard.  So I really only hope that
 * the call setreuid() can drop all privileges, too, on all systems.
 */
int
tino_privilege_end_elevation(void)
{
  int	ret;
  int	tmp;

  ret	= 0;
  if (getegid()!=(tmp=getgid()))
    {
      ret	|= 2;
      setregid(tmp, tmp);
    }
  if (geteuid()!=(tmp=getuid()))
    {
      ret	|= 1;
      setreuid(tmp, tmp);
    }
  return ret;
}


#endif
