/* OS-X fixes
 *
 * This here shall combine special defaults for OS-X.
 *
 * Currently this is a stub based on what was found at http://stackoverflow.com/a/2566110
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_sysfix_osx_h
#define tino_INC_sysfix_osx_h

#ifdef __APPLE__
#ifdef __MACH__

/* This is a stub.  Please fix and fill if you need it.  Thanks. */

#define	TINO_F_fdatasync	fsync

#endif
#endif
#endif
