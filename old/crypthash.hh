/* $Header$
 *
 * Crypt hash classes.
 *
 * UNIT TEST FAILS *
 * NOT READY YET!
 *
 * You must add to makefile
 * -lopenssl
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
 * Revision 1.6  2009-05-21 10:38:15  tino
 * CygWin fixes
 *
 * Revision 1.5  2007-01-25 04:39:15  tino
 * Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
 *
 * Revision 1.4  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.3  2005/08/08 00:40:40  tino
 * define from _h to _hh
 *
 * Revision 1.2  2005/08/08 00:15:04  tino
 * sha
 *
 * Revision 1.1  2005/08/02 04:44:41  tino
 * C++ changes
 */

#ifndef tino_INC_crypthash_hh
#define tino_INC_crypthash_hh

#if 0
#include <string.h>
#endif

#include <string>
using namespace std;

class tino_crypthash
  {
  private:
    virtual void	resetter(void)			= 0;
    virtual void	adder(const void *, size_t)	= 0;
    virtual int		digestlen(void) const		= 0;
    virtual void	digester(void *)		= 0;

  public:
    tino_crypthash()			{ }
    
    void	reset(void)		{ resetter(); }
    void	add(const void *p, size_t l) { adder(p, l); }
    void	add(const char *s)	{ adder(s, strlen(s)); }
    void	add(const string s)	{ adder(s.c_str(), s.length()); }
    string	get(void);

    string	quick(const void *, size_t);
  };

string
tino_crypthash::get(void)
{
  static const char	hex[]	= "0123456789abcdef";
  int			i, len	= digestlen();
  void			*ptr	= alloca(len);
  string		ret;

  digester(ptr);

  for (i=0; i<len; i++)
    {
      ret	+= hex[(((unsigned char *)ptr)[i]>>4)&0xf];
      ret	+= hex[(((unsigned char *)ptr)[i]>>4)&0xf];
    }
  return ret;
}

string
tino_crypthash::quick(const void *ptr, size_t len)
{
  resetter();
  adder(ptr, len);
  return get();
}

/**********************************************************************/

#define TINO_CRYPTHASH(LOWER,UPPER,CTX,LEN,CAST)			\
class tino_##LOWER : public tino_crypthash				\
  {									\
  private:								\
    CTX		ctx;							\
    void	resetter(void)			{ UPPER##_Init(&ctx); }	\
    int		digestlen(void) const		{ return LEN; }		\
    void	adder(const void *, size_t);				\
    void	digester(void *);					\
  public:								\
    tino_##LOWER()				{ resetter(); }		\
  };									\
void									\
tino_##LOWER::adder(const void *ptr, size_t len)			\
{									\
  if (len)								\
    UPPER##_Update(&ctx, CAST ptr, len);				\
}									\
void									\
tino_##LOWER::digester(void *ptr)					\
{									\
  CTX	ctx2;								\
									\
  ctx2	= ctx;								\
  UPPER##_Final((unsigned char *)ptr, &ctx2);				\
}

/**********************************************************************/

#ifndef TINO_CRYPTHASH_OLD
#include <openssl/md2.h>
TINO_CRYPTHASH(md2,		MD2,		MD2_CTX,	MD2_DIGEST_LENGTH, (const unsigned char *))

#include <openssl/md4.h>
TINO_CRYPTHASH(md4,		MD4,		MD4_CTX,	MD4_DIGEST_LENGTH,)
#endif

#include <openssl/md5.h>
TINO_CRYPTHASH(md5,		MD5,		MD5_CTX,	MD5_DIGEST_LENGTH,)

#include <openssl/ripemd.h>
TINO_CRYPTHASH(ripemd160,	RIPEMD160,	RIPEMD160_CTX,	RIPEMD160_DIGEST_LENGTH,)

#include <openssl/sha.h>
#ifndef TINO_CRYPTHASH_OLD
TINO_CRYPTHASH(sha_old,		SHA,		SHA_CTX,	SHA_DIGEST_LENGTH,)
#endif
TINO_CRYPTHASH(sha1,		SHA1,		SHA_CTX,	SHA_DIGEST_LENGTH,)

#endif
