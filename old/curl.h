/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * CURL backend
 *
 * This is only for a single URL use in parallel.
 *
 * Copyright (C)2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
 * Revision 1.6  2007-08-08 11:26:12  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.5  2007/01/25 18:08:23  tino
 * intermediate
 *
 * Revision 1.4  2007/01/25 04:39:15  tino
 * Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
 *
 * Revision 1.3  2007/01/18 20:07:04  tino
 * tino_va_list and TINO_VA_LIST changes
 *
 * Revision 1.2  2006/10/21 01:46:15  tino
 * Commit for save
 *
 * Revision 1.1  2006/08/14 04:21:13  tino
 * Changes for the new added curl.h and data.h
 */

#ifndef tino_INC_curl_h
#define tino_INC_curl_h

#include "data.h"
#include "buf_printf.h"

#include <curl/curl.h>
#include <curl/easy.h>

#define	cDP	TINO_DP_curl

#define tino_curl	tino_curl_sk0wer9s8fj2349j23
static struct tino_curl
  {
    CURL	*handle;
    int		inited;
    TINO_DATA	*verbose;
  } tino_curl;

static void
tino_curl_verbose(const char *s, ...)
{
  tino_va_list	list;

  if (!tino_curl.verbose)
    return;
  tino_va_start(list, s);
  tino_data_vsprintf(tino_curl.verbose, &list);
  tino_va_end(list);
}

static void
tino_curl_init(TINO_DATA *verbose)
{
#ifdef CURLVERSION_NOW
  curl_version_info_data	*data;
#endif

  tino_curl.verbose	= verbose;

  if (tino_curl.inited)
    return;

  curl_global_init(CURL_GLOBAL_ALL);

#ifdef CURLVERSION_NOW
  data	= curl_version_info(CURLVERSION_NOW);
  tino_curl_verbose("using CURL age %d\n", data->age);
  if (data->age>=0)
    {
      tino_curl_verbose("  version %s, host %s, features %x, ssl %s, libz %s\n",
			data->version, data->host, data->features, data->ssl_version, data->libz_version);
      if (data->age>0)
	{
	  tino_curl_verbose("  ares %s", data->ares);
	  if (data->age>1)
	    tino_curl_verbose(", libidn %s", data->libidn);
	}
      tino_curl_verbose("\n");
    }
#else
  tino_curl_verbose("old CURL %s\n", curl_version());
#endif

  tino_curl.inited	= 1;
}

static size_t
tino_curl_write(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t	n;

  n	= size*nmemb;
  tino_data_write(data, ptr, n);
  return n;
}


static int
tino_curl_debug_callback(CURL *curl, curl_infotype type, char *data, size_t len, void *arg)
{
  const char	*prefix;
  int		i;

  prefix	= "unknown";
  switch (type)
    {
    case CURLINFO_TEXT:		prefix="info";		break;
    case CURLINFO_HEADER_IN:	prefix="head in ";	break;
    case CURLINFO_HEADER_OUT:	prefix="head out";	break;
    case CURLINFO_DATA_IN:	prefix="data in ";	break;
    case CURLINFO_DATA_OUT:	prefix="data out";	break;
    }

  for (i=0; i<len; i+=16)
    {
      int	j;

      tino_data_printf(tino_curl.verbose," %s%04llu:", prefix, fmt, pos+i);
      for (j=0; j<16 && i+j<len; j++)
	tino_data_printf(tino_curl.verbose, " %02x", p[i+j]);
      while (++j<=16)
	tino_data_printf(tino_curl.verbose, "   ");
      tino_data_printf(tino_curl.verbose, " ! ");
      for (j=0; j<16 && i+j<len; j++)
	tino_data_printf(tino_curl.verbose, "%c", tino_uni2prn(p[i+j]));
      tino_data_printf(tino_curl.verbose, "\n");
    }
}

static int
tino_curl_get(const char *url, TINO_DATA *out, TINO_DATA *head)
{
  int	ret;

  if (!tino_curl.inited)
    tino_curl_init(NULL);

  if (!tino_curl.handle)
    tino_curl.handle	= curl_easy_init();
  if (!tino_curl.handle)
    tino_exit("cannot get a CURL handle");

#if 0
  curl_easy_reset(tino_curl.handle);
#endif

  if (tino_curl.debug)
    {
      curl_easy_setopt(tino_curl.handle, CURLOPT_DEBUGFUNCTION, tino_curl_debug_callback);
      curl_easy_setopt(tino_curl.handle, CURLOPT_VERBOSE, 1);
    }

  curl_easy_setopt(tino_curl.handle, CURLOPT_NOPROGRESS, 1);

  curl_easy_setopt(tino_curl.handle, CURLOPT_URL, url);
  curl_easy_setopt(tino_curl.handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

  if (out)
    {
      curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEFUNCTION, tino_curl_write);
      curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEDATA, out);
    }
  if (head)
    {
      curl_easy_setopt(tino_curl.handle, CURLOPT_HEADERFUNCTION, tino_curl_write);
      curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEHEADER, head);
    }

  ret	= curl_easy_perform(tino_curl.handle);
  xDP(("tino_curl_getd ret=%d", ret));

  return ret;
}

static void
tino_curl_cleanup(void)
{
  if (!tino_curl.handle)
    return;

  curl_easy_cleanup(tino_curl.handle);
  tino_curl.handle	= 0;
}

#if 0
static void
tino_curl_proxy(const char *s, int type)
{
  tino_curl.proxytype	= type;
  tino_curl.proxy	= s;
}
#endif

#undef tino_curl
#undef cDP
#endif
