/* $Header$
 *
 * Various system fixes I don't want to do all the time again and again
 *
 * Never prototype anything.  Just #define!
 *
 * All other includes shall depend on the defines made in this file, ONLY!
 *
 * Copyright (C)2004-2007 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.6  2007-05-08 03:12:50  tino
 * TINO_F_shutdown added
 *
 * Revision 1.5  2007/04/11 14:25:50  tino
 * See Changelog
 *
 * Revision 1.4  2007/01/28 02:52:49  tino
 * Changes to be able to add CygWin fixes.  I don't think I am ready yet, sigh!
 *
 * Revision 1.3  2006/10/04 00:00:32  tino
 * Internal changes for Ubuntu 64 bit system: va_arg processing changed
 *
 * Revision 1.2  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.1  2004/07/05 23:52:19  tino
 * added
 */

#ifndef tino_INC_sysfix_h
#define tino_INC_sysfix_h

/* For some reason I get errors when including AIO with C++ */
#ifdef  __cplusplus
#define	TINO_NO_INCLUDE_AIO
#endif

#include "sysfix_cygwin.h"


/**********************************************************************/
/* Standard defines from above	*/
#ifndef	TINO_T_
#define	TINO_T_
#endif
#ifndef	TINO_T_
#define	TINO_T_
#endif
#ifndef	TINO_T_
#define	TINO_T_
#endif
#ifndef	TINO_T_socklen_t
#define	TINO_T_socklen_t	socklen_t
#endif
#ifndef	TINO_T_fd_set
#define	TINO_T_fd_set	fd_set
#endif
#ifndef	TINO_T_timeval
#define	TINO_T_timeval	struct timeval
#endif
#ifndef	TINO_T_sockaddr
#define	TINO_T_sockaddr	struct sockaddr
#endif
#ifndef	TINO_T_sockaddr_un
#define	TINO_T_sockaddr_un	struct sockaddr_un
#endif
#ifndef	TINO_T_sockaddr_in
#define	TINO_T_sockaddr_in	struct sockaddr_in
#endif
#ifndef	TINO_T_sockaddr_in6
#define	TINO_T_sockaddr_in6	struct sockaddr_in6
#endif
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
#ifndef	TINO_F_mkdir
#define	TINO_F_mkdir	mkdir
#endif
#ifndef	TINO_F_mkdir
#define	TINO_F_mkdir	mkdir
#endif
#ifndef	TINO_F_chdir
#define	TINO_F_chdir	chdir
#endif
#ifndef	TINO_F_fopen
#define	TINO_F_fopen	fopen64
#endif
#ifndef	TINO_F_freopen
#define	TINO_F_freopen	freopen64
#endif
#ifndef	TINO_F_fdopen
#define	TINO_F_fdopen	fdopen
#endif
#ifndef	TINO_F_open
#define	TINO_F_open	open64
#endif
#ifndef	TINO_F_close
#define	TINO_F_close	close
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
#ifndef	TINO_F_dup2
#define	TINO_F_dup2	dup2
#endif
#ifndef	TINO_F_truncate
#define	TINO_F_truncate	truncate64
#endif
#ifndef	TINO_F_ftruncate
#define	TINO_F_ftruncate	ftruncate64
#endif
#ifndef	TINO_F_fflush
#define	TINO_F_fflush	fflush
#endif
#ifndef	TINO_F_fdatasync
#define	TINO_F_fdatasync	fdatasync
#endif
#ifndef	TINO_F_fcntl
#define	TINO_F_fcntl	fcntl
#endif
#ifndef	TINO_F_mmap
#define	TINO_F_mmap	mmap64
#endif
#ifndef	TINO_F_munmap
#define	TINO_F_munmap	munmap
#endif
#ifndef	TINO_F_read
#define	TINO_F_read	read
#endif
#ifndef	TINO_F_write
#define	TINO_F_write	write
#endif
#ifndef	TINO_F_setsockopt
#define	TINO_F_setsockopt	setsockopt
#endif
#ifndef	TINO_F_htonl
#define	TINO_F_htonl	htonl
#endif
#ifndef	TINO_F_htons
#define	TINO_F_htons	htons
#endif
#ifndef	TINO_F_gethostbyname
#define	TINO_F_gethostbyname	gethostbyname
#endif
#ifndef	TINO_F_alloca
#define	TINO_F_alloca	alloca
#endif
#ifndef	TINO_F_inet_aton
#define	TINO_F_inet_aton	inet_aton
#endif
#ifndef	TINO_F_socket
#define	TINO_F_socket	socket
#endif
#ifndef	TINO_F_bind
#define	TINO_F_bind	bind
#endif
#ifndef	TINO_F_listen
#define	TINO_F_listen	listen
#endif
#ifndef	TINO_F_connect
#define	TINO_F_connect	connect
#endif
#ifndef	TINO_F_inet_ntop
#define	TINO_F_inet_ntop	inet_ntop
#endif
#ifndef	TINO_F_ntohs
#define	TINO_F_ntohs	ntohs
#endif
#ifndef	TINO_F_getpeername
#define	TINO_F_getpeername	getpeername
#endif
#ifndef	TINO_F_getsockname
#define	TINO_F_getsockname	getsockname
#endif
#ifndef	TINO_F_recvfrom
#define	TINO_F_recvfrom	recvfrom
#endif
#ifndef	TINO_F_select
#define	TINO_F_select	select
#endif
#ifndef	TINO_F_fork
#define	TINO_F_fork	fork
#endif
#ifndef	TINO_F_sysconf
#define	TINO_F_sysconf	sysconf
#endif
#ifndef	TINO_F_socketpair
#define	TINO_F_socketpair	socketpair
#endif
#ifndef	TINO_F_accept
#define	TINO_F_accept	accept
#endif
#ifndef	TINO_F_shutdown
#define	TINO_F_shutdown	shutdown
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif
#ifndef	TINO_F_
#define	TINO_F_
#endif

#endif
