/* $Header$
 *
 * Hexdump routine (and hexdump program, just "make test")
 *
 * Copyright (C)2004-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.13  2009-01-08 19:58:57  tino
 * More flexible interface added
 *
 * Revision 1.12  2008-09-01 20:18:15  tino
 * GPL fixed
 *
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

struct tino_xd
  {
    TINO_DATA		*d;
    const char		*prefix;
    int			fmt;
    unsigned long long	pos;
    int			compress, fill, unchanged;
    unsigned char	buf[16];
  };

static void
tino_xd_init(struct tino_xd *xd, TINO_DATA *d, const char *prefix, int fmt, unsigned long long start, int compress)
{
  xd->d		= d;
  xd->prefix	= prefix;
  xd->fmt	= fmt;
  xd->pos	= start;
  xd->compress	= compress;
  xd->fill	= 0;
  xd->unchanged	= 0;
}

static void
tino_xd_flush(struct tino_xd *xd)
{
  if (!xd->compress || !xd->fill)
    return;
  tino_xd(xd->d, xd->prefix, xd->fmt, xd->pos-xd->fill, xd->buf, xd->fill);
  xd->fill	= 0;
}

static void
tino_xd_do(struct tino_xd *xd, const unsigned char *ptr, int len)
{
  int			i, unchanged, fill;
  unsigned char		*buf;
  unsigned long long	pos;

  if (!ptr || !len)
    {
      tino_xd_flush(xd);
      return;
    }
  if (!xd->compress)
    {
      tino_xd(xd->d, xd->prefix, xd->fmt, xd->pos, ptr, len);
      xd->pos	+= len;
      return;
    }

  unchanged	= xd->unchanged;
  fill		= xd->fill;
  buf		= xd->buf;
  pos		= xd->pos;

  for (i=0; i<len; )
    {
      register unsigned char	c;

      if (unchanged==2 && fill==0)
	{
	  while (i+16<=len && !memcmp(ptr+i, buf, 16))
	    {
	      pos	+= 16;
	      i		+= 16;
	    }
	  if (i>=len)
	    break;
	}

      c	=  ptr[i];
      if (buf[fill]!=c)
	{
	  buf[fill]	= c;
	  unchanged	= 0;
	}
      i++;
      pos++;
      if (++fill<16)
	continue;

      if (unchanged==0)
	{
	  tino_xd(xd->d, xd->prefix, xd->fmt, pos-fill, buf, fill);
	  unchanged	= 1;
	  fill		= 0;
	  continue;
	}
      fill	= 0;
      if (unchanged==1)
	tino_data_printfA(xd->d, "*\n");
      unchanged	= 2;
    }

  xd->pos	= pos;
  xd->unchanged	= unchanged;
  xd->fill	= fill;
}

static void
tino_xd_exit(struct tino_xd *xd)
{
  tino_xd_flush(xd);
  if (xd->compress && xd->unchanged)
    tino_xd(xd->d, xd->prefix, xd->fmt, xd->pos, NULL, 0);
}


#ifdef TINO_TEST_MAIN
#undef TINO_TEST_MAIN
#include "getopt.h"
static void
xd(TINO_DATA *d, const char *name, int compress)
{
  FILE	*fd;
  struct tino_xd	xd;
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

  tino_xd_init(&xd, d, "", -8, 0ull, compress);
  while ((n=fread(buf, 1, sizeof buf, fd))>0)
    tino_xd_do(&xd, buf, n);
  tino_xd_exit(&xd);

  if (fd!=stdin)
    fclose(fd);
}

int
main(int argc, char **argv)
{
  TINO_DATA	d = { 0 };
  int		argn, expand;

  argn  = tino_getopt(argc, argv, 1, 0,
                      TINO_GETOPT_VERSION("0.1")
#if 0
                      TINO_GETOPT_DEBUG
#endif
                      " file[..]",

                      TINO_GETOPT_USAGE
                      "h        this help"
		      ,

		      TINO_GETOPT_FLAG
		      "e	expand lines with same content, too"
		      , &expand,

		      NULL);
  if (argn<=0)
    return 1;

  tino_data_file(&d, 1);

  for (; argn<argc; argn++)
    xd(&d, argv[argn], !expand);

  tino_data_freeA(&d);	/* = fclose(stdout);	*/
  return 0;
}
#endif
#endif
