/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * Read simple XML files into memory based on expat.
 *
 * This is for simple XML files like
 *
 * <?xml version="1.0"?>
 * <rows>
 *  <row>
 *   <col>text1</col>
 *   <col>text2</col>
 *  </row>
 *  <row>
 *   <col>text3</col>
 *   <col>text4</col>
 *   <col>text5</col>
 *  </row>
 * </rows>
 *
 * You then can access it like this:
 *
 * TINO_XML x, p;
 * x	= tino_xml_new();
 * tino_xml_read(x, "file.xml");
 * s=tino_xml_get(x, "rows.row.col");	// text1
 * s=tino_xml_get_next(x);		// text2
 * s=tino_xml_get_next(x);		// NULL
 * s=tino_xml_get_next(x);		// text3
 * for (p=tino_xml_first(x, "rows.row"); p; p=tino_xml_next(p))
 *   process(tino_xml_get(p, "col");
 * p=tino_xml_seek(tino_xml_first(&x, "rows.row"), 1);
 * tino_xml_write(p, "file-out.xml");
 * tino_xml_free(p);
 * tino_xml_free(x);
 *
 * Copyright (C)2006-2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 * Revision 1.5  2008-09-01 20:18:15  tino
 * GPL fixed
 *
 * Revision 1.4  2008-05-19 09:14:00  tino
 * tino_alloc naming convention
 *
 * Revision 1.3  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.2  2006/10/04 01:57:12  tino
 * tino_va_* functions for better compatibility
 *
 * Revision 1.1  2006/07/31 23:15:37  tino
 * intermediate version which is not ready
 *
 */

#ifndef tino_INC_xml_h
#define tino_INC_xml_h

#include "err.h"
#include "ex.h"
#include "alloc.h"

#include <stdio.h>
#include <expat.h>

/* never even try to access this, ever
 */
#define	tino_xml_internal	TINO_XML_we0tr934tk205i29jkr2345i

#define cDP	TINO_DP_XML

typedef struct tino_xml	*TINO_XML;

struct tino_xml_internal
  {
    int			refcount;
  };

struct tino_xml_tmp
  {
    XML_Parser		p;
    FILE		*fd;
  };

struct tino_xml
  {
    void		*internal;
    struct tino_xml_tmp	*t;
    int			utf8;
  };

/**********************************************************************/

static void
tino_xml_err(TINO_XML x, const char *s, ...)
{
  tino_va_list	list;
  char		buf[1000];
  int		e;

  e	= errno;
  if (x->t->p)
    snprintf(buf, sizeof buf, "xml parse error line=%d column=%d: %s",
	     XML_GetCurrentLineNumber(x->t->p),
	     XML_GetCurrentColumnNumber(x->t->p),
	     XML_ErrorString(XML_GetErrorCode(x->t->p)));
  else
    snprintf(buf, sizeof buf, "xml error");
  tino_va_start(list, s);
  tino_verror(buf, &list, e);
  tino_va_end(list);
}

/**********************************************************************/

static void
tino_xml_internal_free(struct tino_xml_internal *i)
{
  if (!i)
    return;
  if (--i->refcount>0)
    return;
  000;
  free(i);
}

static struct tino_xml_internal *
tino_xml_internal(TINO_XML x)
{
  struct tino_xml_internal	*i;

  if (!x->internal)
    {
      i			= tino_alloc0O(sizeof *i);
      i->refcount	= 1;
      x->internal	= i;
    }
  return x->internal;
}

/**********************************************************************/

static void
tino_xml_tmp_free(TINO_XML x)
{
  if (!x->t)
    return;
  if (x->t->p)
    XML_ParserFree(x->t->p);
  if (x->t->fd && x->t->fd!=stdin && x->t->fd!=stdout)
    fclose(x->t->fd);
  free(x->t);
  x->t	= 0;
}

static void
tino_xml_tmp_new(TINO_XML x)
{
  tino_xml_tmp_free(x);
  x->t	= tino_alloc0O(sizeof *x->t);
}


/**********************************************************************/

static TINO_XML
tino_xml_new(void)
{
  return tino_alloc0O(sizeof (struct tino_xml));
}

static void
tino_xml_free(TINO_XML x)
{
  if (!x)
    return;

  tino_xml_internal_free(x->internal);
  x->internal	= 0;
  tino_xml_tmp_free(x);
  free(x);
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static void
tino_xml_p_start(void *user, const XML_Char *name, const XML_Char **attr)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_end(void *user, const XML_Char *name)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_data(void *user, const XML_Char *s, int len)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_comment(void *user, const XML_Char *s)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_default(void *user, const XML_Char *s, int len)
{
  TINO_XML	x=user;
}

#if 0
static void
tino_xml_p_exec(void *user, const XML_Char *target, const XML_Char *data)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_cdata(void *user)
{
  TINO_XML	x=user;
}

static void
tino_xml_p_cdata_end(void *user)
{
  TINO_XML	x=user;
}
#endif

static int
tino_xml_parse_step(TINO_XML x, const char *buf, size_t len)
{
  return !XML_Parse(x->t->p, buf, len, buf ? 0 : 1);
}

static int
tino_xml_parse_buf(TINO_XML x, const char *buf, size_t len)
{
  int	i, run;

  run	= 0;
  for (i=0; i<len; i++)
    if (buf[i]>=0 && buf[i]<31 && buf[i]!=10)
      {
	char	tmp[10];

	if (run<i)
	  tino_xml_parse_step(x, buf+run, i-run);
	snprintf(tmp, sizeof tmp, "&#xe0%02x;", buf[i]);
	if (tino_xml_parse_step(x, tmp, strlen(tmp)))
	  return 1;
	run	= i+1;
      }
  if (run<len &&
      tino_xml_parse_step(x, buf+run, len-run))
    return 1;
  return 0;
}

static int
tino_xml_parse_start(TINO_XML x)
{
  XML_Parser	p;

  tino_xml_tmp_new(x);

  p		= XML_ParserCreate(x->utf8 ? "UTF-8" : "ISO-8859-1");
  if (!p)
    {
      tino_err("cannot init expat");
      return 0;
    }

  XML_SetElementHandler(p,			tino_xml_p_start, tino_xml_p_end);
  XML_SetCharacterDataHandler(p,		tino_xml_p_data);
  XML_SetCommentHandler(p,			tino_xml_p_comment);
  XML_SetDefaultHandlerExpand(p,		tino_xml_p_default);
#if 0
  XML_SetProcessingInstructionHandler(p,	tino_xml_p_exec);
  XML_SetCdataSectionHandler(p,			tino_xml_p_cdata, tino_xml_p_cdata_end);
#endif
  XML_SetUserData(p, x);

  x->t->p	= p;
  return 1;
}

static void
tino_xml_parse_end(TINO_XML x)
{
  if (x->t->p)
    tino_xml_parse_step(x, NULL, 0);
  tino_xml_tmp_free(x);
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static int
tino_xml_read(TINO_XML x, const char *name)
{
  FILE	*fd;
  char	buf[BUFSIZ];
  int	got;

  if (!tino_xml_parse_start(x))
    return -1;

  if ((fd=(strcmp(name, "-") ? fopen(name, "rt") : stdin))==0)
    {
      tino_xml_err(x, "cannot open %s", name);
      return -1;
    }
  x->t->fd	= fd;
  while ((got=fread(buf, 1, sizeof buf, fd))>0)
    if (tino_xml_parse_buf(x, buf, got))
      {
	tino_xml_err(x, "parse error %s", name);
	return -1;
      }
  if (ferror(fd))
    {
      tino_xml_err(x, "read error %s", name);
      return -1;
    }
  x->t->fd	= 0;
  if (fd!=stdin && fclose(fd))
    {
      tino_xml_err(x, "close error %s", name);
      return -1;
    }
  tino_xml_parse_end(x);
  return 0;
}


#ifdef TINO_TEST_MAIN
#undef TINO_TEST_MAIN
#include "getopt.h"

int
main(int argc, char **argv)
{
  TINO_XML	x, p;
  int		argn;

  argn  = tino_getopt(argc, argv, 2, 3,
                      TINO_GETOPT_VERSION("0.0.0")
                      " file-in.xml xml.path [file-out.xml]",

                      TINO_GETOPT_USAGE
                      "h        this help"
		      ,
		      NULL);
  if (argn<=0)
    return 1;

  x	= tino_xml_new();
  if (tino_xml_read(x, argv[argn]))
    return 2;
  p	= tino_xml_first(x, argv[argn+1]);
  if (!p)
    {
      tino_err("missing XML path: %s in %s", argv[argn+1], argv[argn]);
      return 3;
    }
  if (argn+2>=argc)
    {
      000;
    }
  else if (tino_xml_write(p, argv[argn+2]))
    return 4;
  return 0;
  
}
#endif

#undef tino_xml_internal
#undef	cDP
#endif
