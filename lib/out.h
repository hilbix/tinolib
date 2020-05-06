/* MiniLib
 *
 * Output formatter
 */

#include "def.h"

#include <stdint.h>

#include "va.h"

#define	OUTput(FD, ...)	OUTput_(outWriter, (void *)(FD), ##__VA_ARGS__, NULL)

#define	OUTlf	"\n"
#define	OUT(X)		OUT_TYPE(X), X
#define	OUTbase(X)	(void *)OUT_BASE, (int)(X)
#define	OUTw(X)		(void *)OUT_WIDTH, (int)(X)
#define	OUTfill(X)	(void *)OUT_FILL, (char)(X)

typedef int (*outCB)(void *user, const void *, size_t);	/* return 0:ok else:ERR	*/

{{MAP mods}}
BASE	base(X)	int	base  = va_arg(list, int)
WIDTH	w(X)	int	width = va_arg(list, int)
FILL	fill(X)	char	fill  = (char)va_arg(list, int)
SIGN	sign(X)	char	sign  = fill
PLUS	plus(X)	-	sign  = '+'
{{.}}

{{LOOP mods}}
#define OUT#2##3# (void *)OUT_{{1}}
{{.}}

{{MAP types}}
ULL	unsigned long long
SLL	long long
ULONG	unsigned long
SLONG	long
UINT	unsigned
SINT	int
UCHAR	unsigned char
SCHAR	char
STR	const char *
VA	struct VA_LIST *
{{.}}

{{SET block}}
{{LOOP types}}
, {{2}}: (void *)OUT_{{1}}
{{.}}
{{.}}
#define	OUT_TYPE(X)	_Generic((X){{types}}

enum
  {
  OUT_NULL = 0,

{{LOOP mods types}}
  OUT_{{1}},
{{.}}
  };

static int ioWrite(int, const void *, int);	/* avoid recursion	*/

static int
outWriter(void *user, const void *ptr, size_t len)
{
  int fd = (int)(uintptr_t)user;

  return ioWrite(fd, ptr, len);
}

struct OutSet
  {
    int		base;	/* default: 10	*/
    int		width;	/* default: 0	*/
    int		fill;	/* default: ' '	*/
    int		sign;	/* default: 0	*/

#if 0
    const char	*s;
    VA_LIST	list;
#endif

    outCB	cb;
    void	*user;
  };

static int
OutPut(struct OutSet *c, va_list list)
{
  const char	*s;

  while ((s=va_arg(list, void *))!=0)
    {
      switch ((uintptr_t)s)
        {
        case OUT_NULL:	return;	/* should not happen	*/
{{LOOP mods}}
case OUT_#1#:
{{.}}
        case OUT_BASE:	c->continue;
        case OUT_WIDTH:	c->continue;
        case OUT_FILL:	c->continue;
        case OUT_SIGN:	c->continue;
        case OUT_PLUS:	c->continue;

        case OUT_STR:	OutStr(c, va_arg(list, const char *)); continue;;

        default:	/* string	*/
          n	= strlen(s);
          FORMATfill(fill, width-n, cb, user);
          cb(user, s, n);
          goto out;
        }
    }
  return 0;
}

static int
OUTput_(outCB cb, void *user, ...)
{
  struct OutSet	c = { 0 };
  int		ret;
  va_list	list;

  c.user= user;
  c.cb	= cb;
  va_list(list, user);

  ret	= OutPut(&c, list);

  va_end(list);
  return ret;
}

#if 0
    {
      int			type;
      long long			ll  = 0;	/* shutup compiler	*/
      unsigned long long	ull = 0;	/* shutup compiler	*/
      size_t			n;

        {
        char c;

        case FORMAT_ARGS:	s	= va_arg(list, const char *); vFORMAT(cb, user, s, *va_arg(list, va_list *)); continue;
        /* THIS ^^^^^ is missing in printf(), so I need to re-invent the wheel, sigh.	*/
        case FORMAT_NULL:	return;
        default:


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
