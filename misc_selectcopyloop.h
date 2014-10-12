/* A simple select-copy-loop between 2 sockets.
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#ifndef	DP
#define	DP(X)	do { ; } while (0)
#endif
#ifndef XD
#define	XD(STR,IDX,PTR,LEN)
#endif


static void
selectcopyloop_closew(int fd)
{
  if (fd>=0)
    {
      shutdown(fd, 1);
      close(fd);
    }
}

static void
selectcopyloop_closer(int fd)
{
  if (fd>=0)
    {
      shutdown(fd, 0);
      close(fd);
    }
}

static const char *
selectcopyloop_sockopt(int sock)
{
  int			on;
  struct linger		linger;

  on = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on))
    return "setsockopt reuse";
  linger.l_onoff	= 1;
  linger.l_linger	= 65535;
  if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof linger))
    return "setsockopt linger";
  on	= 102400;
  if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &on, sizeof on))
    return "setsockopt sndbuf";
  on	= 102400;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &on, sizeof on))
    return "setsockopt rcvbuf";

#if 0
  if (fcntl(sock, F_SETFL, O_NDELAY))
    return "O_NDELAY";
#endif

  return 0;
}

static const char *
selectcopyloop(int in, int sock, int out, int throttle, int mtu, long long *total)
{
  struct transfer
  {
    int		ok;
    int		from, to;
    int		pos, fill;
    long	throttle;
    char	buf[BUFSIZ];
  } move[2];
  int			max, block, sock2;
  struct timeval	now;
  long			throtmul, throtdiv, throtmax;
  const char		*s;

  if (throttle && gettimeofday(&now, NULL))
    {
      perror("gettimeofday");
      throttle	= 0;
    }

  sock2		= dup(sock);
  if (sock2<0)
    return "dup";
  if ((s=selectcopyloop_sockopt(sock2))!=0)
    return s;

  block	= BUFSIZ;
  if (throttle && block>throttle)
    block	= throttle;
  if (!mtu || mtu>BUFSIZ)
    mtu	= BUFSIZ;
  if (throttle && mtu>throttle)
    mtu	= throttle;

  throtmax	= throttle/5;
  if (throtmax<mtu)
    throtmax	= mtu;
  if (throttle && throtmax>throttle)
    throtmax	= throttle;

  throtdiv	= 1000;
  throtmul	= throttle;
  while (throtdiv>1 && throtmul>=1000000)
    {
      throtmul	/= 10;
      throtdiv	/= 10;
    }

  move[0].ok		= 1;
  move[0].from		= in;
  move[0].to		= sock;
  move[0].pos		= 0;
  move[0].fill		= 0;
  move[0].throttle	= throtmax;

  move[1].ok		= 1;
  move[1].from		= sock2;
  move[1].to		= out;
  move[1].pos		= 0;
  move[1].fill		= 0;
  move[1].throttle	= throtmax;

  max	= sock2;
  if (max<=sock)
    max	= sock;
  if (max<=in)
    max	= in;
  if (max<=out)
    max	= out;
  max++;

  DP(("throtmul=%ld throtdiv=%ld block=%d mtu=%d max=%d",
      throtmul, throtdiv, block, mtu, max));

  while (move[0].ok || move[1].ok)
    {
      fd_set		fdin, fdout, fdex;
      struct timeval	timeout, *timeoutp, *timeoutn;
      int		i;
      long		throtval;

      FD_ZERO(&fdin);
      FD_ZERO(&fdout);
      FD_ZERO(&fdex);

      timeoutp	= 0;
      timeoutn	= 0;
      throtval	= 0;
      if (throttle)
	{
	  struct timeval	tmp;
	  long			delta;

	  gettimeofday(&tmp, NULL);
	  delta	= (tmp.tv_usec-now.tv_usec)/1000;
	  if (tmp.tv_sec!=now.tv_sec)
	    {
	      now.tv_sec	= tmp.tv_sec-1;
	      delta		+= 1000;
	    }

	  throtval	= delta*throtmul/throtdiv;
	  if (throtval<0)
	    throtval	= 0;
	  if (throtval>=throtmax)
	    throtval	= throtmax;
	  DP(("throtval %ld", throtval));

	  delta		= throtval*throtdiv/throtmul;
	  DP(("delta %ld", delta));
	  now.tv_usec	+= delta*1000;
	  if (now.tv_usec>=1000000)
	    {
	      now.tv_sec	+= 1;
	      now.tv_usec	-= 1000000;
	    }

	  timeout.tv_sec	= 0;
	  timeout.tv_usec	= 100000;
	  timeoutn		= &timeout;
	}

      for (i=2; --i>=0; )
	if (move[i].ok)
	  {
	    if (move[i].throttle<throttle)
	      {
		move[i].throttle	+= throtval;
		DP(("throttle %d=%d", i, move[i].throttle));
		timeoutp		=  timeoutn;
	      }
	    if (move[i].fill>move[i].pos)
	      {
		if (move[i].throttle>0)
		  {
		    DP(("set out %d", move[i].to));
		    FD_SET(move[i].to, &fdout);
		  }
	      }
	    else if (move[i].from<0)
	      {
		DP(("finish %d", i));
		selectcopyloop_closew(move[i].to);
		move[i].ok	= 0;
		continue;
	      }
	    FD_SET(move[i].to, &fdex);
	    if (move[i].from>=0)
	      {
		FD_SET(move[i].from, &fdex);
		if (move[i].pos || move[i].fill<block)
		  {
		    DP(("set in %d", move[i].from));
		    FD_SET(move[i].from, &fdin);
		  }
	      }
	  }

      if (!timeoutp && !move[0].ok && !move[1].ok)
	break;

      if (select(max, &fdin, &fdout, &fdex, timeoutp)<0)
	{
	  if (errno==EINTR)
	    continue;
	  return "select";
	}

      for (i=2; --i>=0; )
	if (move[i].ok)
	  {
	    if (FD_ISSET(move[i].to, &fdout))
	      {
		int	put;

		DP(("put %d", move[i].to));
		put	= move[i].fill-move[i].pos;
		if (put>mtu)
		  put	= mtu;
		if (put>move[i].throttle)
		  put	= move[i].throttle;
		DP(("toput %d", put));
		put	= write(move[i].to, move[i].buf+move[i].pos, put);
		DP(("putted %d", put));
		if (put>=0)
		  {
		    XD((i ? "s>" : "1>"), move[i].pos, move[i].buf+move[i].pos, put);
		    move[i].pos	+= put;
		    if (throttle)
		      move[i].throttle	-= put;
		  }
		else if (errno!=EINTR)
		  {
		    DP(("close %d", i));
		    selectcopyloop_closew(move[i].to);
		    selectcopyloop_closer(move[i].from);
		    move[i].ok	= 0;
		    break;
		  }
	      }
	    else if (FD_ISSET(move[i].to, &fdex))
	      {
		DP(("ex %d", move[i].to));
		selectcopyloop_closew(move[i].to);
		selectcopyloop_closer(move[i].from);
		move[i].ok	= 0;
		break;
	      }
	    if (move[i].from>=0)
	      {
		if (FD_ISSET(move[i].from, &fdin))
		  {
		    int	get;

		    DP(("get %d", move[i].from));
		    if (move[i].fill<=move[i].pos)
		      {
			move[i].pos		= 0;
			move[i].fill	= 0;
		      }
		    if (move[i].pos && move[i].fill+mtu>BUFSIZ)
		      {
			int	len;

			len		= move[i].fill-move[i].pos;
			memmove(move[i].buf, move[i].buf+move[i].pos, len);
			move[i].pos	= 0;
			move[i].fill	= len;
		      }
		    get	= BUFSIZ-move[i].fill;
		    DP(("toget %d", get));
		    if (get>0)
		      {
			get	= read(move[i].from, move[i].buf+move[i].fill, get);
			DP(("got %d", get));
			if (get>0)
			  {
			    XD((i ? "0<" : "s<"), move[i].fill, move[i].buf+move[i].fill, get);
			    move[i].fill	+= get;
			    if (total)
			      *total	+= get;
			  }
			else if (!get || errno!=EINTR)
			  {
			    DP(("shutdown %d", i));
			    selectcopyloop_closer(move[i].from);
			    move[i].from	= -1;
			    break;
			  }
		      }
		  }
		else if (FD_ISSET(move[i].from, &fdex))
		  {
		    DP(("ex %d", move[i].from));
		    selectcopyloop_closer(move[i].from);
		    move[i].from	= -1;
		    break;
		  }
	      }
	  }
    }
  return 0;
}
