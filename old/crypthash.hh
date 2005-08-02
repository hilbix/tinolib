/* $Header$
 *
 * Crypt hash class.
 *
 * You must add to makefile
 * -lopenssl
 *
 * $Log$
 * Revision 1.1  2005-08-02 04:44:41  tino
 * C++ changes
 *
 */

#ifndef tino_INC_crypthash_h
#define tino_INC_crypthash_h

#include <string.h>

#include <openssl/md5.h>

class tino_md5
  {
  private:
    MD5_CTX	ctx;
    char	tmp[34];

    tino_md5();

  public:
    void	add(const void *, size_t);
    void	add(const char *);
    const char	*get(void);
  };

tino_md5::tino_md5()
{
  MD5_Init(&ctx);
}

void
tino_md5::add(const void *ptr, size_t len)
{
  if (len)
    MD5_Update(&ctx, ptr, len);
}

void
tino_md5::add(const char *s)
{
  add(s, strlen(s));
}

const char *
tino_md5::get(void)
{
  static const char	hex[]	= "0123456789abcdef";
  unsigned char		digest[16];
  char			*ptr;
  int			i;
  MD5_CTX		ctx2;

  ctx2			= ctx;
  MD5_Final(digest, &ctx2);

  ptr	= tmp;
  for (i=0; i<16; i++)
    {
      *ptr++	= hex[(digest[i]>>4)&0xf];
      *ptr++	= hex[(digest[i]>>0)&0xf];
    }
  *ptr		= 0;
  return tmp;
}

#endif
