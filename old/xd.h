/* $Header$
 *
 * $Log$
 * Revision 1.6  2005-03-15 18:18:37  tino
 * no more fatal.h, now TINOCOPY=tino/xd.h works
 *
 * Revision 1.5  2005/01/26 12:18:51  tino
 * xd output enhanced - UNIT_TEST is a hexdumper
 *
 * Revision 1.4  2004/09/04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.3  2004/03/28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.1  2004/03/26 19:58:04  tino
 * added
 */

#ifndef tino_INC_xd_h
#define tino_INC_xd_h

#include <stdio.h>

static int
tino_uni2prn(unsigned c)
{
  if (c>0xff)
    return '?';
  if (c<0x20 || (c>=0x7f && c<0xa0))
    return '.';
  return c;
}

static void
tino_xd(FILE *fd, const char *prefix, int fmt, unsigned long long pos, const unsigned char *p, int len)
{
  int	i;

  if (!p || !len)
    {
      fprintf(fd, "%s%0*llu:\n", prefix, fmt, pos);
      return;
    }
  for (i=0; i<len; i+=16)
    {
      int	j;

      fprintf(fd, "%s%0*llu:", prefix, fmt, pos+i);
      for (j=0; j<16 && i+j<len; j++)
	fprintf(fd, " %02x", p[i+j]);
      while (++j<=16)
	fprintf(fd, "   ");
      fprintf(fd, " ! ");
      for (j=0; j<16 && i+j<len; j++)
	fprintf(fd, "%c", tino_uni2prn(p[i+j]));
      fprintf(fd, "\n");
    }
}

#ifdef TINO_TEST_MAIN
#undef TINO_TEST_MAIN
#include "getopt.h"
static void
xd(const char *name)
{
  FILE	*fd;
  char	buf[BUFSIZ];
  long	pos;
  int	n;

  fd	= stdin;
  if (strcmp(name, "-"))
    {
      fd	= fopen(name, "rb");
      if (!fd)
	{
	  perror(name);
	  return;
	}
    }
  pos	= 0;
  while ((n=fread(buf, 1, sizeof buf, fd))>0)
    {
      tino_xd(stdout, "", 8, pos, buf, n);
      pos	+= n;
    }
  if (fd!=stdin)
    fclose(fd);
}

int
main(int argc, char **argv)
{
  int	argn;

  argn  = tino_getopt(argc, argv, 1, 0,
                      TINO_GETOPT_VERSION("0.1")
#if 0
                      TINO_GETOPT_DEBUG
#endif
                      " file[..]",

                      TINO_GETOPT_USAGE
                      "h        this help"
		      ,
		      NULL);
  if (argn<=0)
    return 1;

  for (; argn<argc; argn++)
    xd(argv[argn]);
  return 0;
}
#endif
#endif
