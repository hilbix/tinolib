/* $Header$
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * CURL backend
 *
 * This is only for a single URL use in parallel.
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
 * Revision 1.8  2009-03-17 10:30:16  tino
 * Still not ready version
 *
 * Revision 1.7  2008-09-01 20:18:13  tino
 * GPL fixed
 *
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

#include "xd.h"
#include "err.h"
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
    int		debug;
    const char	*proxy, *proxyauth;
  } tino_curl;

static void
tino_curl_verbose(const char *s, ...)
{
  tino_va_list	list;

  if (!tino_curl.verbose)
    return;
  tino_va_start(list, s);
  tino_data_vsprintfA(tino_curl.verbose, &list);
  tino_va_end(list);
}

static void
tino_curl_init(TINO_DATA *verbose, int debug)
{

#ifdef CURLVERSION_NOW
  curl_version_info_data	*data;
#endif

  tino_curl.verbose	= verbose;
  tino_curl.debug	= debug;

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
  tino_data_writeA(data, ptr, n);
  return n;
}


static int
tino_curl_debug_callback(CURL *curl, curl_infotype type, char *data, size_t len, void *arg)
{
  const char	*prefix;

  prefix	= "unknown";
  switch (type)
    {
    case CURLINFO_TEXT:		prefix="I ";	break;
    case CURLINFO_HEADER_IN:	prefix="H<";	break;
    case CURLINFO_HEADER_OUT:	prefix="H>";	break;
    case CURLINFO_DATA_IN:	prefix="D<";	break;
    case CURLINFO_DATA_OUT:	prefix="D>";	break;
    case CURLINFO_SSL_DATA_IN:	prefix="S<";	break;
    case CURLINFO_SSL_DATA_OUT:	prefix="S>";	break;
    case CURLINFO_END:		prefix="E ";	break;
    }

  tino_xd(tino_curl.verbose, prefix, -4, 0ull, (const unsigned char *)data, len);
  return 0;
}

static int
tino_curl_get(const char *url, TINO_DATA *out, TINO_DATA *head)
{
  int	ret;

  DP(("(url=%s,out=%p,head=%p)", url, out, head));

  if (!tino_curl.inited)
    tino_curl_init(NULL, 0);

  if (tino_curl.handle)
    curl_easy_reset(tino_curl.handle);
  else
    {
      tino_curl.handle	= curl_easy_init();
      if (!tino_curl.handle)
	{
	  tino_err(TINO_ERR(FTLCU100F,) "cannot get CURL handle");
	  return -1;
	}
    }

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
  DP(("() ret=%d", ret));
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

static void
tino_curl_proxy(const char *proxy, const char *proxyauth)
{
  tino_curl.proxy	= proxy;
  tino_curl.proxyauth	= proxyauth;
}

#undef tino_curl
#undef cDP
#endif
