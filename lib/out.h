/* MiniLib
 *
 * Output formatter
 */

#include "def.h"

#include <stdint.h>

#include "va.h"

typedef void (*outCB)(void *user, const void *, size_t);

#define	OUTlf	"\n"

static void
outWriter(void *user, const void *ptr, size_t len)
{
  void ioWrite(int, const char *, int len);	/* avoid recursion	*/

  ioWrite((int)(uintptr_t)user, ptr, len);
}

#define	OUTwrite(FD, ...)	OUTput_(outWriter, (void *)(FD), ##__VA_ARGS__, NULL)

#define	OUT_TYPE(X)	_Generic((X)			\
	, unsigned long long:	(void *)OUT_ULL		\
	, long long:		(void *)OUT_SLL		\
	, unsigned long:	(void *)OUT_ULONG	\
	, long:			(void *)OUT_SLONG	\
	, unsigned:		(void *)OUT_UINT	\
	, int:			(void *)OUT_SINT	\
	, unsigned char:	(void *)OUT_UCHAR	\
	, char:			(void *)OUT_SCHAR	\
	, const char *:		(void *)OUT_STR		\
	, struct VA_LIST *:	(void *)OUT_VA		\
	)

#define	OUT(X)	OUT_TYPE(X), X

enum
  {
  OUT_NULL = 0,

  OUT_BASE,
  OUT_WIDTH,
  OUT_FILL,
  OUT_SIGN,
  OUT_PLUS,

  OUT_STR,
  OUT_VA,

  OUT_I8,
  OUT_I16,
  OUT_I32,
  OUT_I64,
  OUT_SCHAR,
  OUT_SINT,
  OUT_SLONG,
  OUT_SLL,

  OUT_U8,
  OUT_U16,
  OUT_U32,
  OUT_U64,
  OUT_UCHAR,
  OUT_UINT,
  OUT_ULONG,
  OUT_ULL,
  };

static void
OUTput_(outCB cb, void *user, const char *s, ...)
{
  VA_LIST	list;

  VA_START(list, s);
  000;
  VA_END(list);
}

#if 0
/* XXX TODO XXX Some formats from above are missing for now	*/
#define	fVA(s,v)	(void *)FORMAT_ARGS, (const char *)s, &v
#define	fCHAR(X)	(void *)FORMAT_CHAR, (char)(X)
#define	fINT(X)		(void *)FORMAT_INT, (int)(X)
#define	fLONG(X)	(void *)FORMAT_LONG, (long)(X)
#define	fLL(X)		(void *)FORMAT_LL, (long long)(X)
#define	fU8(X)		(void *)FORMAT_U8, (uint8_t)(X)
#define	fU16(X)		(void *)FORMAT_U16, (uint16_t)(X)
#define	fU32(X)		(void *)FORMAT_U32, (uint32_t)(X)
#define	fU64(X)		(void *)FORMAT_U64, (uint64_t)(X)
#define	fULONG(X)	(void *)FORMAT_ULONG, (unsigned long long)(X)
#define	fULL(X)		(void *)FORMAT_ULL, (unsigned long long)(X)

#define	fBASE(X)	(void *)FORMAT_BASE, (int)(X)
#define	fWIDTH(X)	(void *)FORMAT_WIDTH, (int)(X)
#define	fFILL(X)	(void *)FORMAT_FILL, (char)(X)

static size_t
FORMATnr(char *buf, size_t len, unsigned long long n, int base)
{
  const char	*b;

  if (base<0)
    {
      base	= -base;
      if (base<-64)
        b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#$%&()*+-;<=>?@^_`{|}~";	/* RFC1924	*/
      else if (base<-62)
        b	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";				/* B64url	*/
      else
        b	= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";				/* HEX	*/
    }
  else
    {
      if (base>64)
        b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";	/* Z85	*/
      else if (base>62)
        b	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";				/* B64	*/
      else
        b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";				/* hex	*/
    }

#if 0
  buf[--len]	= 0;
#endif
  if (!n)
    {
      buf[--len]	= b[0];
      return len;
    }

  if (base<0)	base	= -base;
  if (base<2)	base	= 2;
  if (base>85)	base	= 85;

  do
    {
      buf[--len]	= b[n%base];
    } while (n /= base);

  return len;
}

static void
FORMATfill(char fill, int width, void (*cb)(void *user, const void *, size_t len), void *user)
{
  char	buf[1024];

  if (width < 0)
    return;
  memset(buf, fill, sizeof buf);
  while ((width -= sizeof buf)>0)
    cb(user, buf, sizeof buf);
  cb(user, buf, width + sizeof buf);
}

/* We need this to not put the big buffer from stack within the recursion
 */
static size_t
FORMATsigned(int type, unsigned long long ull, long long ll, int base, int width, int fill, int sign, void (*cb)(void *user, const void *, size_t), void *user)
{
  char		tmp[PATH_MAX];	/* this is the big buffer we want to avoid	*/
  size_t	n;

  if (type==1)
    {
      if (ll<0)
        {
          sign	= '-';
          ull	= -ll;
        }
      else
        ull	= ll;
    }

  /* If fill is too big, pre-fill as this uses tmp buffer
   */
  n	= sizeof tmp - 8*sizeof ull;
  if (width > n)
    {
      FORMATfill(fill, width - n, cb, user);
      width	= n;
    }

  /* get number	*/
  n	= FORMATnr(tmp, sizeof tmp, ull, base);
  if (sign)
    tmp[--n]	= sign;

  /* fill it on the left	*/
  while (n > sizeof tmp - width && n)
    tmp[--n]	= fill;

  /* output	*/
  cb(user, tmp+n, sizeof tmp - n);
  return n;
}

/* No floating point for now
 */
static void
vFORMAT(void (*cb)(void *user, const void *, size_t len), void *user, const char *s, va_list list)
{
  int	base=10, width=0, fill=' ', sign=0;

  for (;; s=va_arg(list, void *))
    {
      int			type;
      long long			ll  = 0;	/* shutup compiler	*/
      unsigned long long	ull = 0;	/* shutup compiler	*/
      size_t			n;

      switch ((uintptr_t)s)
        {
	char c;

        case FORMAT_ARGS:	s	= va_arg(list, const char *); vFORMAT(cb, user, s, *va_arg(list, va_list *)); continue;
        /* THIS ^^^^^ is missing in printf(), so I need to re-invent the wheel, sigh.	*/
        case FORMAT_NULL:	return;
        default:
          n	= strlen(s);
          FORMATfill(fill, width-n, cb, user);
          cb(user, s, n);
          goto out;

        case FORMAT_BASE:	base	= va_arg(list, int);		continue;
        case FORMAT_WIDTH:	width	= va_arg(list, int);		continue;
        case FORMAT_FILL:	fill	= (char)va_arg(list, int);	continue;
        case FORMAT_SIGN:	sign	= fill;				continue;
        case FORMAT_PLUS:	sign	= '+';				continue;

        case FORMAT_CHAR:	c	= va_arg(list, int); cb(user, &c, 1); continue;

        case FORMAT_I8:		ll	= (int8_t)va_arg(list, int);	type=1; break;
        case FORMAT_I16:	ll	= (int16_t)va_arg(list, int);	type=1; break;
        case FORMAT_I32:	ll	= va_arg(list, int32_t);	type=1; break;
        case FORMAT_I64:	ll	= va_arg(list, int64_t);	type=1; break;
        case FORMAT_INT:	ll	= va_arg(list, int);		type=1; break;
        case FORMAT_LONG:	ll	= va_arg(list, long);		type=1; break;
        case FORMAT_LL:		ll	= va_arg(list, long);		type=1; break;

        case FORMAT_U8:		ull	= (uint8_t)va_arg(list, int);	type=2; break;
        case FORMAT_U16:	ull	= (uint16_t)va_arg(list, int);	type=2; break;
        case FORMAT_U32:	ull	= va_arg(list, uint32_t);	type=2; break;
        case FORMAT_U64:	ull	= va_arg(list, uint64_t);	type=2; break;
        case FORMAT_UCHAR:	ull	= (unsigned char)va_arg(list, int);		type=2; break;
        case FORMAT_UINT:	ull	= va_arg(list, unsigned int);	type=2; break;
        case FORMAT_ULONG:	ull	= va_arg(list, unsigned long);	type=2; break;
        case FORMAT_ULL:	ull	= va_arg(list, unsigned long);	type=2; break;
        }
      n	= FORMATsigned(type, ull, ll, base, width, fill, sign, cb, user);

out:
      /* fill it on the right	*/
      FORMATfill(fill, -width-n, cb, user);

      /* reset format after output	*/
      base=10, width=0, fill=' ', sign=0;
    }
}

#define	FORMAT(X...)	_FORMAT(X, NULL)
static void
_FORMAT(void (*cb)(void *user, const void *, size_t len), void *user, const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  vFORMAT(cb, user, s, list);
  va_end(list);
}
#endif
