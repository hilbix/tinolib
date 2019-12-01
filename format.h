/* Easy to use recoursive output formatter.
 *
 * DO NOT USE tino_format_UPPERCASE from outside.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */
enum
  {
  tino_format_null = 0,

  tino_format_base,
  tino_format_width,
  tino_format_fill,
  tino_format_sign,
  tino_format_plus,

  tino_format_args,

  tino_format_i8,
  tino_format_i16,
  tino_format_i32,
  tino_format_i64,
  tino_format_char,
  tino_format_int,
  tino_format_long,
  tino_format_ll,

  tino_format_u8,
  tino_format_u16,
  tino_format_u32,
  tino_format_u64,
  tino_format_uchar,
  tino_format_uint,
  tino_format_ulong,
  tino_format_ull,

  tino_format_dump,
  };

/* XXX TODO XXX Many formats from above are missing for now	*/
#define	TF_args(s,v)	(void *)tino_format_args,	(const char *)s, &v
#define	TF_dump(p,l)	(void *)tino_format_dump,	(const void *)p, (size_t)l
#define	TF_char(X)	(void *)tino_format_char,	(char)(X)
#define	TF_int(X)	(void *)tino_format_int,	(int)(X)
#define	TF_u8(X)	(void *)tino_format_u8,		(uint8_t)(X)
#define	TF_u16(X)	(void *)tino_format_u16,	(uint16_t)(X)
#define	TF_u32(X)	(void *)tino_format_u32,	(uint32_t)(X)
#define	TF_u64(X)	(void *)tino_format_u64,	(uint64_t)(X)
#define	TF_ull(X)	(void *)tino_format_ull,	(unsigned long long)(X)
#define	TF_base(X)	(void *)tino_format_base,	(int)(X)
#define	TF_width(X)	(void *)tino_format_width,	(int)(X)
#define	TF_fill(X)	(void *)tino_format_fill,	(char)(X)

static int
tino_format_BASE(int base, const char **b)
{
  if (base<0)
    {
      base	= -base;
      if (base > 64)
        *b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#$%&()*+-;<=>?@^_`{|}~";	/* RFC1924	*/
      else if (base > 62)
        *b	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";				/* B64url	*/
      else
        *b	= "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";				/* HEX	*/
    }
  else
    {
      if (base > 64)
        *b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";	/* Z85	*/
      else if (base > 62)
        *b	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";				/* B64	*/
      else
        *b	= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";				/* hex	*/
    }
  return base < 2 ? 2 : base > 85 ? 85 : base;
}

static void
tino_format_FILL(void (*cb)(void *user, const void *, size_t len), void *user, char fill, int width)
{
  char	buf[1024];

  if (width < 0)
    return;
  memset(buf, fill, sizeof buf);
  while ((width -= sizeof buf)>0)
    cb(user, buf, sizeof buf);
  cb(user, buf, width + sizeof buf);
}

static void
tino_format_DUMP85(void (*cb)(void *user, const void *, size_t), void *user, const char *ptr, size_t len, const char *b, int width, int fill, int sign)
{
  tino_fatal("base85 not yet implemented");
}

static void
tino_format_DUMP_2(void (*cb)(void *user, const void *, size_t), void *user, const char *ptr, size_t len, const char *b, int width, int fill, int sign, int base, int bits)
{
  int	mask	= base-1;

  while (len)
    {
      int	n;
      unsigned	c;

      len--;
      c	= (unsigned char)*ptr++;
      for (n=8;;)
        {
          n	-= bits;
          cb(user, &b[(c>>n)&mask], 1);
          if (!n)
            break;
        }
    }
}

static void
tino_format_DUMP_1(void (*cb)(void *user, const void *, size_t), void *user, const char *ptr, size_t len, const char *b, int width, int fill, int sign, int base, int bits)
{
  tino_fatal("base %d not yet implemented", base);
}

static void
tino_format_DUMPff(void (*cb)(void *user, const void *, size_t), void *user, const void *ptr, size_t len, int width, int fill, int sign)
{
  tino_format_FILL(cb, user, sign, -width+len);
  cb(user, ptr, len);
  tino_format_FILL(cb, user, fill, width-len);
}

static void
tino_format_DUMP(void (*cb)(void *user, const void *, size_t), void *user, const void *ptr, size_t len, int base, int width, int fill, int sign)
{
  const char	*b;

  if (base==256)
    return tino_format_DUMPff(cb, user, ptr, len, width, fill, sign);

  base	= tino_format_BASE(base, &b);
  switch (base)
    {
    default:	tino_fatal("not implemented dump base %d", base);

    case 85:	return tino_format_DUMP85(cb, user, ptr, len, b, width, fill, sign);

    case 2:	return tino_format_DUMP_2(cb, user, ptr, len, b, width, fill, sign, base, 1);
    case 4:	return tino_format_DUMP_2(cb, user, ptr, len, b, width, fill, sign, base, 2);
    case 16:	return tino_format_DUMP_2(cb, user, ptr, len, b, width, fill, sign, base, 4);

    case 8:	return tino_format_DUMP_1(cb, user, ptr, len, b, width, fill, sign, base, 3);
    case 32:	return tino_format_DUMP_1(cb, user, ptr, len, b, width, fill, sign, base, 5);
    case 64:	return tino_format_DUMP_1(cb, user, ptr, len, b, width, fill, sign, base, 6);
    }
}

static size_t
tino_format_NR(char *buf, size_t len, unsigned long long n, int base)
{
  const char	*b;

  base	= tino_format_BASE(base, &b);

#if 0
  buf[--len]	= 0;
#endif
  if (!n)
    {
      buf[--len]	= b[0];
      return len;
    }

  do
    {
      buf[--len]	= b[n%base];
    } while (n /= base);

  return len;
}

/* We need this to not put the big buffer from stack within the recursion
 */
static size_t
tino_format_SIGNED(void (*cb)(void *user, const void *, size_t), void *user, int type, unsigned long long ull, long long ll, int base, int width, int fill, int sign)
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
      tino_format_FILL(cb, user, fill, width - n);
      width	= n;
    }

  /* get number	*/
  n	= tino_format_NR(tmp, sizeof tmp, ull, base);
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
tino_format_V(void (*cb)(void *user, const void *, size_t len), void *user, const char *s, va_list list)
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

        case tino_format_args:	s	= va_arg(list, const char *); tino_format_V(cb, user, s, *va_arg(list, va_list *)); continue;
        /* THIS ^^^^^ is missing in printf(), so I need to re-invent the wheel, sigh.	*/
        case tino_format_null:	return;
        default:
          n	= strlen(s);
          tino_format_FILL(cb, user, fill, width-n);
          cb(user, s, n);
          goto out;

        case tino_format_dump:
          s	= va_arg(list, const void *);
          tino_format_DUMP(cb, user, s, va_arg(list, size_t), base, width, fill, sign);
          goto reset;

        case tino_format_base:	base	= va_arg(list, int);		continue;
        case tino_format_width:	width	= va_arg(list, int);		continue;
        case tino_format_fill:	fill	= (char)va_arg(list, int);	continue;
        case tino_format_sign:	sign	= fill;				continue;
        case tino_format_plus:	sign	= '+';				continue;

        case tino_format_char:	c	= va_arg(list, int); cb(user, &c, 1); continue;

        case tino_format_i8:	ll	= (int8_t)va_arg(list, int);	type=1; break;
        case tino_format_i16:	ll	= (int16_t)va_arg(list, int);	type=1; break;
        case tino_format_i32:	ll	= va_arg(list, int32_t);	type=1; break;
        case tino_format_i64:	ll	= va_arg(list, int64_t);	type=1; break;
        case tino_format_int:	ll	= va_arg(list, int);		type=1; break;
        case tino_format_long:	ll	= va_arg(list, long);		type=1; break;
        case tino_format_ll:	ll	= va_arg(list, long);		type=1; break;

        case tino_format_u8:	ull	= (uint8_t)va_arg(list, int);	type=2; break;
        case tino_format_u16:	ull	= (uint16_t)va_arg(list, int);	type=2; break;
        case tino_format_u32:	ull	= va_arg(list, uint32_t);	type=2; break;
        case tino_format_u64:	ull	= va_arg(list, uint64_t);	type=2; break;
        case tino_format_uchar:	ull	= (unsigned char)va_arg(list, int);		type=2; break;
        case tino_format_uint:	ull	= va_arg(list, unsigned int);	type=2; break;
        case tino_format_ulong:	ull	= va_arg(list, unsigned long);	type=2; break;
        case tino_format_ull:	ull	= va_arg(list, unsigned long);	type=2; break;
        }
      n	= tino_format_SIGNED(cb, user, type, ull, ll, base, width, fill, sign);

out:
      /* fill it on the right	*/
      tino_format_FILL(cb, user, fill, -width-n);

reset:
      /* reset format after output	*/
      base=10, width=0, fill=' ', sign=0;
    }
}

#define	tino_format(X...)	tino_FORMAT(X, NULL)
static void
tino_FORMAT(void (*cb)(void *user, const void *, size_t len), void *user, const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_format_V(cb, user, s, list);
  va_end(list);
}

static void
tino_format_fwrite(void *user, const void *ptr, size_t len)
{
  fwrite(ptr, len, (size_t)1, user);
}

#define	tino_format_stderr(X...)	tino_format_STDERR(X, NULL)
static void
tino_format_STDERR(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_format_V(tino_format_fwrite, stderr, s, list);
  va_end(list);
}

#define	tino_format_stdout(X...)	tino_format_STDOUT(X, NULL)
static void
tino_format_STDOUT(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_format_V(tino_format_fwrite, stdout, s, list);
  va_end(list);
}

#define	tino_format_stdout_flush(X...)	tino_format_STDOUT_FLUSH(X, NULL)
static void
tino_format_STDOUT_FLUSH(const char *s, ...)
{
  va_list	list;

  va_start(list, s);
  tino_format_V(tino_format_fwrite, stdout, s, list);
  va_end(list);
  fflush(stdout);
}

