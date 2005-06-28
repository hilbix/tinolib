/* $Header$
 *
 * Scylla+Charybdis library: Future home of all the SC encapsulation.
 * Copyright (C)2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 * 
 * Unlike other libraries, this has prefix sc_ and not tino_
 *
 * THIS LIBRARY IS NOT READY YET!  It's just functional for one single case!
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.1  2005-06-28 20:10:28  tino
 * started to add IOW (IO wrapper)
 *
 */

#ifndef tino_INC_sc_lib_h
#define tino_INC_sc_lib_h

#include "iow_socket.h"
#include "debug.h"

#if 0
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

/* Incremented if significant handshake changes
 */
#define SC_LIB_VERSION 0

struct sc_lib_info
  {
    char		workmode;	/* single character telling of the sc_lib phase	*/

    int			count;
    int			match, copy, notfound, errors;

    unsigned long long	net_in, net_out;	/* Data transferred over network	*/

    unsigned long long	file_pos, file_size;	/* current file size and pos, estimates	*/
  };

typedef struct sc_lib *sc_lib_ctx;
struct sc_lib
  {
    int			sock;

    /* Callback functions.
     * There currently is no error handling within the library,
     * so they must provide the error handling themself.
     * All callbacks share the same user defined pointer.
     */
    void		*user;

    /* Connect to scylla.
     * Must be a file handle of any kind.
     */
    int			(*connect)(void *);
    int			(*read)(int, void *, size_t max);
    int			(*write)(int, const void *, size_t max);

    /* do the alarm ticker callback (each second): */
    int			(*tick)(sc_lib_ctx *, void *);
  };

static void
sc_lib_init(sc_lib_ctx ctx, void *user)
{
  memset(ctx, 0, sizeof *ctx);
  ctx->user	= user;
}

static const struct sc_lib_info *
sc_lib_info(sc_lib_ctx ctx)
{
  return &ctx->info;
}

#endif
