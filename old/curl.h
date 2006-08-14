/* $Header$
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
 * Revision 1.1  2006-08-14 04:21:13  tino
 * Changes for the new added curl.h and data.h
 *
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
    TINO_BUF	verbose_buf;
    void	(*verbose_fn)(TINO_BUF *, void *user);
    void	*verbose_fn_user;
  } tino_curl;

static void
tino_curl_verbose(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_buf_add_vsprintf(&tino_curl.verbose_buf, s, list);
  va_end(list);
  if (tino_curl.verbose_fn && *s && s[strlen(s)-1]=='\n')
    tino_curl.verbose_fn(&tino_curl.verbose_buf, tino_curl.verbose_fn_user);
}

static void
tino_curl_verbose_set(void (*fn)(TINO_BUF *, void *user), void *user)
{
  tino_curl.verbose_fn_user	= user;
  tino_curl.verbose_fn		= fn;
}

static void
tino_curl_init(void)
{
#ifdef CURLVERSION_NOW
  curl_version_info_data	*data;
#endif

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
tino_curl_get(const char *url, TINO_DATA *out, TINO_DATA *head)
{
  int	ret;

  tino_curl_init();
  
  tino_curl.handle	= curl_easy_init();
  if (!tino_curl.handle)
    tino_exit("cannot get a CURL handle");

#if 0
  curl_easy_reset(tino_curl.handle);
#endif

#if 0
  if (tino_curl.debug)
    curl_easy_setopt(tino_curl.handle, CURLOPT_VERBOSE, 1);
#endif
  curl_easy_setopt(tino_curl.handle, CURLOPT_NOPROGRESS, 1);

  curl_easy_setopt(tino_curl.handle, CURLOPT_URL, url);
  curl_easy_setopt(tino_curl.handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

  curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEFUNCTION, tino_curl_write);
  curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEDATA, out);

  curl_easy_setopt(tino_curl.handle, CURLOPT_HEADERFUNCTION, tino_curl_write);
  curl_easy_setopt(tino_curl.handle, CURLOPT_WRITEHEADER, head);

  ret	= curl_easy_perform(tino_curl.handle);
  DP(("tino_curl_getd ret=%d", ret));

#if 0
  /* There must be an error in the libcurl description
   *
   * It says (at CURLFORM_BUFFERTR) it must be called, and says simultanously
   * (curl_easy_cleanup) that you must not call it to keep the connection alive
   */
  curl_easy_cleanup(dris_curl_hndl);
#endif

  return ret;
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
