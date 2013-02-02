/* $Header$
 *
 * THIS IS NOT READY YET *
 * UNIT TEST FAILS *
 *
 * Shell Helper Integrated Transfer
 *
 * Copyright (C)2008-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.5  2009-01-27 14:58:18  tino
 * Single FD mode, still not ready
 *
 * Revision 1.4  2008-11-02 01:52:03  tino
 * TINO_GETOPT_TYPE_IGNORE
 *
 * Revision 1.3  2008-09-01 20:18:14  tino
 * GPL fixed
 *
 * Revision 1.2  2008-05-29 22:19:37  tino
 * Example corrected
 */

#ifndef tino_INC_shit_h
#define tino_INC_shit_h

#include "alloc.h"

#if 0
/* This is an integration method to fork off some shell helpers as
 * co-processes in a standard way.  The helpers need only be forked
 * once or even can run on remote systems via sockets to perform their
 * tasks, as all commandline parameters etc. are transferred via
 * stdin.  Stdout and stderr are left as they are.
 *
 * SHIT mode is run with the --shit option, which can be invoked via
 * tino_getopt() with following part:
 *
 *	TINO_GETOPT_IGNORE TINO_GETOPT_LLOPT TINO_GETOPT_FN
 *	"shit	Shell Helper Integrated Transfer (do not use)"
 *	, shit_mode,
 *
 * This calls a code snippet as follows (note: the args are not used):
 */
const char *
shit_mode(void *ptr, const char *arg, const char *opt, void *usr)
{
  struct tino_shit	shit;
  struct tino_shit_io	*me, *r;

  tino_shit_initO(&shit, "HELPER_NAME");
  me	= tino_shit_helperO(&shit, 0, getenv("TINO_SHIT_MODE"));
  if (!me)
    return "SHIT mode cannot be used manually";

  while ((r=tino_shit_request_inN(me))!=0)
    {
      tino_shit_answer_spoolO(r);
      process_string(tino_shit_stringO(r));
      tino_shit_answer_finishO(r);
      tino_shit_closeO(r);
    }
  tino_shit_exitO(&shit, 0);	// Free resources, exit(0)
  return 0;
}

/* The server side is likewise easy to use:
 */
struct tino_shit	shit;
struct tino_shit_io	*helper;

tino_shit_initO(&shit, "INTEGRATOR_NAME");
helper	= tino_shit_fork_argsN(&shit, "/path/to/program", "--shit", NULL);

// here use send_request()

tino_shit_closeO(helper);	// Close the helper
tino_shit_endO(&shit);		// Free resources

/* Send a request to the helper and receive the response
 */
const char *
send_request(const char *req)
{
  struct tino_shit_io	*r;

  r	= tino_shit_request_outO(helper);
  tino_shit_put_stringO(r, req);
  tino_shit_sendO(r);
  answer	= strdup(tino_shit_stringO(r));
  tino_shit_closeO(r);
  return answer;
}

/* You can fork off more than one helper in parallel, you can have
 * more than one request active in parallel, and you can send and
 * receive more than one argument and variably terminated argument
 * lists.
 *
 * Any communication error (like squence errors) immediately
 * terminates the helper and the connection to it.  If automatic error
 * handling is not overwritten, the integrator (the one calling the
 * helpers) terminates, too, to be on the safe side.
 */
#endif

struct tino_shit
  {
    const char		*name;
    struct tino_shit_io	*io;
  };

struct tino_shit_io
  {
    struct tino_shit_io	*next;
    struct tino_shit	*shit;
  };

/* Close the handle, free resources and return exit termination state
 * (exit status) of the connection
 */
static int
tino_shit_closeO(struct tino_shit_io *io)
{
  000;
  return -1;
}

/* Terminate the SHIT structure
 */
static void
tino_shit_endO(struct tino_shit *shit)
{
  while (shit->io)
    tino_shit_closeO(shit->io);
  tino_free_constO(shit->name);
  shit->name	= 0;
  000;
}

static void
tino_shit_exitO(struct tino_shit *shit, int exit_val)
{
  000;	/* XXX send exit code to the other side	*/
  tino_shit_endO(shit);
  exit(exit_val);
}

static void
tino_shit_initO(struct tino_shit *shit, const char *name)
{
  shit->io	= 0;
  shit->name	= tino_strdupO(name);
  000;
}

/* Start a helper.
 *
 * IO is the FD used for IO (usually 0)
 * ID is what was transferred via getopt("TINO_SHIT_MODE")
 */
static struct tino_shit_io *
tino_shit_helperO(struct tino_shit *shit, int io, const char *id)
{
  000;
  return 0;
}

/* Receive a request.
 *
 * Usually done on the helper side.
 */
static struct tino_shit_io *
tino_shit_request_inN(struct tino_shit_io *helper)
{
  000;
  return 0;
}

/* Start spooling the answer.
 *
 * This means, the helper starts sending arbitrary data to stdout.
 *
 * This is implemented by telling some (hopefully) random string to
 * the other side which does not show up in the data.
 */
static void
tino_shit_answer_spoolO(struct tino_shit_io *io)
{
  000;
}

/* Stop spooling the answer.
 *
 * The data is terminated by the above string.
 */
static void
tino_shit_answer_finishO(struct tino_shit_io *io)
{
  000;
}

/* This returns the next string from the IO.
 *
 * Note that this string is kept in memory until the request is
 * closed by tino_shit_closeO() or tino_shit_freeO()
 */
static const char *
tino_shit_stringO(struct tino_shit_io *io)
{
  000;
  return 0;
}

/* Free the string which was allocated by tino_shit_stringO()
 *
 * Often you do not need to do this, as this is implicitly done with
 * tino_shit_closeO()
 */
static void
tino_shit_freeO(struct tino_shit_io *io, const void *s)
{
  000;
}

#endif
