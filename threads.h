/* Thread safety
 *
 * This is just some code to handle thread safety.
 * If your program uses threads,
 * #define	TINO_THREAD_SUPPORT	TINO_THREAD_PTHREAD_FAST
 * This shall fix the moste problematic things with threads.
 * Note that tinolib is not (yet) supposed to be thread aware.
 *
 * Copyright (C)2005-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef tino_INC_threads_h
#define tino_INC_threads_h

#define	TINO_THREAD_PTHREAD_FAST	0x974cead1ul

#ifndef TINO_THREAD_SUPPORT

#define TINO_THREAD_SEMAPHORE(X)	
#define TINO_THREAD_SEMAPHORE_GET(X)	do { ; } while (0)
#define TINO_THREAD_SEMAPHORE_FREE(X)	do { ; } while (0)

#elif TINO_THREAD_SUPPORT==TINO_THREAD_PTHREAD_FAST


#include <pthread.h>

#define TINO_THREAD_SEMAPHORE(X)	static pthread_mutex_t	X=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#define TINO_THREAD_SEMAPHORE_GET(X)	pthread_mutex_lock(&(X))
#define TINO_THREAD_SEMAPHORE_FREE(X)	pthread_mutex_unlock(&(X))

#else
#error "unknown thread support type, sorry"
#endif

#endif
