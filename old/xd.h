/* $Header$
 *
 * Hexdump routine (and hexdump program, just "make test")
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.11  2007-09-26 21:29:46  tino
 * make test works again
 *
 * Revision 1.10  2007/09/18 02:29:51  tino
 * Bugs removed, see ChangeLog
 *
 * Revision 1.9  2007/09/17 17:45:10  tino
 * Internal overhaul, many function names corrected.  Also see ChangeLog
 *
 * Revision 1.8  2007/08/08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.7  2005/12/05 02:11:13  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.6  2005/03/15 18:18:37  tino
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

#include "data.h"

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
tino_xd(TINO_DATA *d, const char *prefix, int fmt, unsigned long long pos, const unsigned char *p, int len)
{
  int	i;

  if (!p || !len)
    {
      if (fmt<0)
	tino_data_printfA(d, "%s%0*llx:\n", prefix, -fmt, pos);
      else
	tino_data_printfA(d, "%s%0*llu:\n", prefix, fmt, pos);
      return;
    }
  for (i=0; i<len; i+=16)
    {
      int	j;
      char	buf[80], *ptr;

      tino_data_putsA(d, prefix);
      if (fmt<0)
	tino_data_printfA(d, "%0*llx:", -fmt, pos+i);
      else
	tino_data_printfA(d, "%0*llu:", fmt, pos+i);
      ptr	= buf;
      for (j=0; j<16 && i+j<len; j++)
	{
	  unsigned char	c=p[i+j];

	  *ptr++	= ' ';
	  *ptr++	= "0123456789abcdef"[(c>>4)&0xf];
	  *ptr++	= "0123456789abcdef"[c&0xf];
	}
      while (++j<=16)
	{
	  *ptr++	= ' ';
	  *ptr++	= ' ';
	  *ptr++	= ' ';
	}
      *ptr++	= ' ';
      *ptr++	= '!';
      *ptr++	= ' ';
      for (j=0; j<16 && i+j<len; j++)
	*ptr++	= tino_uni2prn(p[i+j]);
      *ptr++	= '\n';
      *ptr	= 0;
      tino_data_putsA(d, buf);
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
  TINO_DATA	d;

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
  tino_data_file(&d, 1);
  pos	= 0;
  while ((n=fread(buf, 1, sizeof buf, fd))>0)
    {
      tino_xd(&d, "", 8, pos, buf, n);
      pos	+= n;
    }
  if (fd!=stdin)
    fclose(fd);
  tino_data_freeA(&d);	/* fclose(stdout);	*/
}

int
main(int argc, char **argv)
{
  int		argn;

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
