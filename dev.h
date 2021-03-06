/* Device file magic
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

#ifndef tino_INC_dev_h
#define tino_INC_dev_h

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "strprintf.h"

static int
tino_dev_getaddr(int fd, const char *name, struct sockaddr *sa)
{
  struct ifreq	ifr;
  int		s, ret;

  if ((s=fd)<0 && (s=socket(AF_INET, SOCK_DGRAM, 0))<0)
    return -1;

  ifr.ifr_addr.sa_family	= AF_INET;
  tino_strxcpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
  ret	= ioctl(fd, SIOCGIFADDR, &ifr);
  if (fd<0)
    close(s);

  /* The interface either does not exist
   * or has no IP set up.
   * In this case it's unusable anyway.
   * You have to setup the interface before calling this.
   */
  if (ret)
    return -1;

  *sa	= ifr.ifr_addr;
  return 0;
}

/* Open a device like /dev/tap
 * Return IP of the device in sa.
 * If sa==NULL it may be that the device returned is not usable
 * (has no IP set).
 *
 * Luckily we need not rely on /proc
 * as this changed over time.
 */
static int
tino_dev_open(const char *name, struct sockaddr *sa)
{
  char	tmp[IF_NAMESIZE+10];
  int	fd;

  if (strlen(name)>=IF_NAMESIZE)
    tino_exit("tap device name too long");
  sprintf(tmp, "/dev/%s", name);

  if ((fd=open(tmp, O_RDWR|O_NOCTTY))<0)
    return -1;

  /* Is the device usable?
   */
  if (sa && tino_dev_getaddr(fd, name, sa))
    {
      close(fd);
      return -1;
    }
  return fd;
}

#endif
