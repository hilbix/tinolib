/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-06-17 21:07:50  tino
 * added
 *
 */

#ifndef tino_INC_dev_h
#define tino_INC_dev_h

static int
tino_dev_getaddr(int fd, const char *name, struct sockaddr *sa)
{
  int	s;

  if ((s=fd)<0 && (s=socket(AF_INET, SOCK_DGRAM, 0))<0)
    return -1;

  ifr.ifr_addr.sa_family	= AF_INET;
  strcpy(ifr.ifr_name, dev);
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
 * Return IP of the device in sa
 *
 * Luckily we need not rely on /proc
 * as this changed over time.
 */
static int
tino_dev_open(const char *name, struct sockaddr *sa)
{
  char		tmp[IF_NAMESIZE+10];
  struct ifreq	ifr;
  int		fd;

  if (strlen(name)>=IF_NAMESIZE)
    ex("tap device name too long");
  sprintf(tmp, "/dev/%s", name);

  if ((fd=open(tmp, O_RDWR|O_NOCTTY))<0)
    return -1;

  if (sa)
    {
    }
}

#endif
