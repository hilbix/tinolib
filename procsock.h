/* Create a socket, possibly to a program
 *
 * Copyright (C)2010-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_proc_sock_h
#define tino_INC_proc_sock_h

#include "sock.h"
#include "proc.h"
#include "strargs.h"

#define	cDP	TINO_DP_proc

/** Create program, TCP or Unix socket
 *
 * If prefix is given it must be present to create a program socket.
 * If prefix==NULL always a prograp socket is created.
 *
 * In case of exec "bind" is the environment
 */
static int
tino_proc_sock(const char *connect, const char *prefix, const char *bind)
{
  const char	*run;
  int		sp[2];
  pid_t		chi;
  tino_str_args	arg, env;

  cDP(("(%s,%s,%s)", connect, prefix, bind));

  run	= tino_strprefixcmp2_const(connect, prefix ? prefix : "");
  if (!run)
    {
      cDP(("() trying socket"));
      return tino_sock_tcp_connect(connect, bind && *bind ? bind : NULL);
    }

  tino_sock_pairA(sp);

  cDP(("() sp %d,%d", sp[0],sp[1]));

  tino_str_args_init(&env,bind);
  tino_str_args_init(&arg,run);
  chi	= tino_fork_exec(sp[0],sp[0],2,tino_str_args_argv(&arg),tino_str_args_env(&env, "UNNAMED"),1,NULL);
  tino_str_args_free(&arg);
  tino_str_args_free(&env);

  cDP(("() chi=%ld", (long)chi));

  tino_file_closeE(sp[0]);
  cDP(("() ret %d", sp[1]));
  return sp[1];
}

#undef	cDP
#endif
