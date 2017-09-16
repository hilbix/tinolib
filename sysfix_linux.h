/* Linux fixes
 *
 * This here shall combine special defaults for Linux glibc.
 *
 * To be able to be compiled on Linux variants from the last millenium,
 * this uses the xxxxx64 variants for many of the functions.
 *
 * Perhaps I find some way to do this only for old systems.
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_sysfix_linux_h
#define tino_INC_sysfix_linux_h

#ifdef __gnu_linux__

#include <sys/syscall.h>	/* SYS_renameat2	*/
#include <linux/fs.h>		/* renameat2	*/

#ifndef	TINO_T_off_t
#define	TINO_T_off_t	off64_t
#endif
#ifndef	TINO_T_fpos_t
#define	TINO_T_fpos_t	fpos64_t
#endif
#ifndef	TINO_T_stat
#define	TINO_T_stat	struct stat64
#endif
#ifndef	TINO_F_stat
#define	TINO_F_stat	stat64
#endif
#ifndef	TINO_F_lstat
#define	TINO_F_lstat	lstat64
#endif
#ifndef	TINO_F_fstat
#define	TINO_F_fstat	fstat64
#endif
#ifndef	TINO_F_fopen
#define	TINO_F_fopen	fopen64
#endif
#ifndef	TINO_F_freopen
#define	TINO_F_freopen	freopen64
#endif
#ifndef	TINO_F_open
#define	TINO_F_open	open64
#endif
#ifndef	TINO_F_ftello
#define	TINO_F_ftello	ftello64
#endif
#ifndef	TINO_F_fseeko
#define	TINO_F_fseeko	fseeko64
#endif
#ifndef	TINO_F_fgetpos
#define	TINO_F_fgetpos	fgetpos64
#endif
#ifndef	TINO_F_fsetpos
#define	TINO_F_fsetpos	fsetpos64
#endif
#ifndef	TINO_F_truncate
#define	TINO_F_truncate	truncate64
#endif
#ifndef	TINO_F_ftruncate
#define	TINO_F_ftruncate	ftruncate64
#endif
#ifndef	TINO_F_lseek
#define	TINO_F_lseek	lseek64
#endif

#endif
#endif
