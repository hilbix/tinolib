/* NOT READY YET!
 * UNIT TEST FAILS *
 *
 * Scylla+Charybdis library
 *
 * Copyright (C)2000-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_sc_lib_h
#define tino_INC_sc_lib_h

#include "alarm.h"
#include "buf_printf.h"
#include "md5.h"

#include <sys/time.h>

typedef struct tino_sc	*TINO_SC;
enum tino_sc_state
  {
    TINO_SC_STATE0='/',
    TINO_SC_STATE1='-',
    TINO_SC_STATE2='\\',
    TINO_SC_STATE3='|',
  };

struct tino_sc
  {
    int		sock;
    void	(*progress)(TINO_SC, int state);
    const char	*version, *ssl, *pw;
    void	*user;
    TINO_BUF	buf;
    int		timeout;
  };

/**********************************************************************/

/**********************************************************************/

static void
tino_sc_open(TINO_SC sc, const char *version, int sock, const char *ssl, const char *pw, void *user)
{
  sc->version	= version;
  sc->sock	= sock;
  sc->ssl	= ssl;
  sc->pw	= pw;
  sc->user	= user;
}

static void
tino_sc_close(TINO_SC sc)
{
  tino_alarm_stop(NULL, sc);
  000;
  close(sc->sock);
  sc->sock	= -1;
}

/**********************************************************************/

static void
tino_sc_send_buf(TINO_SC sc, const char *s, size_t len)
{
  int	put;

  sc->timeout	= 0;
  for (pos=0; pos<len; )
    {
      size_t	max;

      
    }
}

static void
tino_sc_send(TINO_SC sc, const char *s)
{
  tino_sc_send_buf(sc, s, strlen(s));
  tino_sc_send_buf(sc, "\r\n", 2);
}

static const char *
tino_sc_line(TINO_SC sc)
{
  000;
}


/**********************************************************************/

/* Will call the progress_fn each second
 * and will alter 'state' on progress.
 */
static void
tino_sc_set_progress(TINO_SC sc, void (*progress_fn)(TINO_SC, int))
{
  sc->progress	= progress_fn;
  tino_alarm_set(1, tino_sc_progress_handler, sc);
}

/**********************************************************************/

static void *
tino_sc_get_user(TINO_SC sc)
{
  return sc->user;
}

/**********************************************************************/

static void
tino_sc_preamble(TINO_SC sc)
{
  struct timeval	tv;
  char			tmp[33];

  gettimeofday(&tv, NULL);
  tino_buf_add_sprintf(&sc->buf, "%ld %ld %ld %ld", tv.tv_sec, tv.tv_usec, (long)getpid(), (long)getppid());
  tino_md5_str(tmp, tino_buf_get_s(&sc->buf));
  tino_buf_add_sprintf(&sc->buf, "SCYLLA %s %s", sc->version, tmp);
  tino_sc_send(sc, tino_buf_get_s(&sc->buf));
}

/**********************************************************************/

#if 0

/* Old
 */
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

#endif
