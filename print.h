/*
 * My very own version of printf.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 *
 * 1) vprintf(str, va_list) is written as tino_print(ctx, "%v", TINO_VA_LIST)
 * 2) There are no sideffects on the TINO_VA_LIST
 * 3) The format is slightly different to printf!
 *
 * This currently is a stub!
 * "%v" currently is only supported at the beginning or end of a format string!
 * Everything else is handled with vsnprintf()
 *
 * Format: %[!][[+|-| ][0]xval][.xval][ext..][width][type]
 * Format: %{future}
 *
 * !: argument is ignored (not rendered)
 * width: hh h l ll
 * xval: * or val
 * val: Number or '(expr)'
 * type:
 * - %		percent-character
 * - c		character
 * - di		Integer
 * - n		offset (stores number of characters output so far)
 * - ouxX	octal unsinged hex HEX
 * - ps		Pointer C-String
 * types (NEW):	ANSI/SUS/ISO uses so far: #%'+-0123456789 aAcCdeEfFgGhiIjlLmnopsStuxXz
 * - P		void *, size_t
 * - V		visual character: 'c', "'", NUL, etc.
 * - v		TINO_VA_LIST: v	this is a recursion!
 * - B		TINO_BUF *
 * ext:
 * - @[val]:	argument number for reordering. * is not supported here.
 * %{future}:
 * - Future extension, reserved
 *
 * Not supported:
 *
 * - ' I	locales
 * - aAeEfFgGL	Floating Point
 * - CjmqStz	whatever
 * - lc ls	wchar_t
 */

/* @ extension explained:
 *
 * @ must come after size.
 *
 * @0 is the same as @ and denotes the same argument:
 * tino_print(NULL, "Dec %d Octal %@o Hex %@x\n", 42);
 *
 * @* is not supported for apparently reasons.  Else it works straight
 * forward, a * in the size is fetched before the argument as usual
 * and is not counted.  Note that you must give the stars on the first
 * definition, later you can leave them away.  Example:
 * tino_print(NULL, "Short %-*.*s: Long %-*@s\n", 5, 4, "thisisalongtext");
 *
 * There may not be any holes, following is an error:
 * tino_print(NULL, "%@3d\n", "hello", 5, 42)
 * To fix that with:
 * tino_print(NULL, "%!s%!d%d\n", "hello", 5, 42)
 * Also following would be ok then:
 * tino_print(NULL, "%@3d %@2d %@1s\n", "hello", 5.5, 42)
 */

/* %v extension explained:
 *
 * A TINO_VA_LIST is fetched like any other object (this is a pointer).
 *
 * It then is rendered recursively (as the formatting is in the TINO_VA_LIST).
 *
 * A size is not yet supported.
 *
 * Indexing is not yet supported.  Indexing might look like: %=5d
 */

#ifndef tino_INC_print_h
#define tino_INC_print_h

#include "buf_printf.h"

typedef struct tino_print_ctx	*TINO_PRINT_CTX;
struct tino_print_ctx
  {
    TINO_BUF	buf;
    int		err;
    int		(*fn)(TINO_PRINT_CTX, const void *, int);	/* BUF,len>=0 | NULL,byte>=0 | NULL,-1 flush	*/
    uintptr_t	u;
  };

static int
tino_print_buf(TINO_BUF *buf, const void *p, int l)
{
  if (p)
    tino_buf_add_nO(buf, p, l);
  else if (l>=0)
    tino_buf_add_cO(buf, l);
  return 0;
}

static int
tino_print__self(TINO_PRINT_CTX c, const void *p, int l)
{
  return tino_print_buf(&c->buf, p, l);
}

static int
tino_print_err(TINO_PRINT_CTX c, int err)
{
  DP(("(%p, %d)", c, err));
  return err>=0
         ? c ? c->err     : -1
         : c ? c->err=err : err;
}

static int
tino_print__fn(TINO_PRINT_CTX c, const void *ptr, int len)
{
  return !c || c->err
         ? tino_print_err(c, len)
         : (c->err = (c->fn ? c->fn : tino_print__self)(c, ptr, len));
}

static int
tino_print__flush(TINO_PRINT_CTX c)
{
  return tino_print__fn(c, NULL, -1);
}

static int
tino_print__c(TINO_PRINT_CTX c, char chr)
{
  return tino_print__fn(c, NULL, ((int)chr)&0xff);
}

static int
tino_print__p(TINO_PRINT_CTX c, const void *s, int len)
{
  return s && len>=0 ? tino_print__fn(c, s, len) : tino_print_err(c, len);
}

static int
tino_print__s(TINO_PRINT_CTX c, const char *s)
{
  return tino_print__fn(c, s, strlen(s));
}

/* Initialize a context.
 *
 * Please note that C either can be NULL to allocate a context,
 * or it must be pre-initialized with NULL.
 */
static TINO_PRINT_CTX
tino_print_ctxO(TINO_PRINT_CTX *c)
{
  TINO_PRINT_CTX ctx;

  if (c && *c)
    return *c;
  ctx	= tino_alloc0O(sizeof *ctx);
  if (c)
    *c	= ctx;
  return ctx;
}

static void
tino_print_freeO(TINO_PRINT_CTX *c)
{
  TINO_PRINT_CTX	ctx;

  if (!c || !*c) return;
  ctx	= *c;
  *c	= 0;

  TINO_XXX;

  tino_buf_freeO(&ctx->buf);
  tino_freeO(ctx);
}

#define	TINO_PRINT_CTX(TYPE, NAME, CODE)			\
        static int tino_print_fn_##NAME(TINO_PRINT_CTX ctx, const void *data, int len) { TYPE NAME=(TYPE)ctx->u; return CODE; }	\
        static TINO_PRINT_CTX					\
        tino_print_ctx_##NAME(TINO_PRINT_CTX c, TYPE NAME)	\
        {							\
          tino_print_ctxO(&c);					\
          tino_buf_resetO(&c->buf);				\
          c->u	= (uintptr_t)NAME;				\
          c->fn	= tino_print_fn_##NAME;				\
          return c;						\
        }

TINO_PRINT_CTX(TINO_BUF *,buf,	tino_print_buf(buf, data, len))
TINO_PRINT_CTX(FILE *,fd,	(data ? len ? 1==fwrite(data,len,1, fd) : len : len>=0 ? EOF==fputc(len,fd) : fflush(fd)))
TINO_PRINT_CTX(int,io,		data ? tino_io_write(io,data,len) : len>=0 ? tino_io_put(io,len) : tino_io_flush_write(io))

static TINO_PRINT_CTX tino_vprintO(TINO_PRINT_CTX c, TINO_VA_LIST list);

static TINO_PRINT_CTX
tino_vprint_(TINO_PRINT_CTX c, TINO_VA_LIST list)
{
  const char	*s;

  xDP(("(%p %p) %p:'%s'%p", c, list, list, TINO_VA_STR(list), TINO_VA_GET(list)));
  tino_print_ctxO(&c);
  xDP(("() %p", c));

  while (tino_str_startswith(s, "%v"))
    {
      TINO_VA_LIST	src;

      src	= TINO_VA_ARG(list, TINO_VA_LIST);
      xDP(("() %%v=%p:'%s'%p", src, TINO_VA_STR(src), TINO_VA_GET(src)));
      tino_vprintO(c, src);
      s	+= 2;
    }
  if (!*s)
    return c;
  if (tino_str_endswith(s, "%v"))
    {
      char		*tmp;
      TINO_VA_LIST	src;

      tmp			= tino_strdupO(s);
      tmp[strlen(tmp)-2]	= 0;
      TINO_VA_STR(list)		= tmp;

      tino_vprint_imp(c, list);
      tino_freeO(tmp);
      TINO_VA_STR(list)		= 0;

      src	= TINO_VA_ARG(list, TINO_VA_LIST);
      xDP(("() %%v=%p:'%s'%p", src, TINO_VA_STR(src), TINO_VA_GET(src)));
      return tino_vprintO(c, src);
    }

  /* adjust the format pointer	*/
  TINO_VA_STR(list)	= s;

  /* Just do a normal printf for now	*/
  s	= tino_str_vprintf(list);
  tino_print__s(c, s);
  tino_free_constO(s);

  return c;
}

static TINO_PRINT_CTX
tino_vprintO(TINO_PRINT_CTX c, TINO_VA_LIST list)
{
  tino_va_list  list2;

  tino_va_copy(list2, *list);
  xDP(("(%p %p) %p:'%s'%p %p:'%p'%p", c, list, list, TINO_VA_STR(list), TINO_VA_GET(list), &list2, tino_va_str(list2), tino_va_get(list2)));
  c	= tino_vprint_imp(c, &list2);
  xDP(("() %p %p:'%s'%p %p:'%p'%p", c, list, TINO_VA_STR(list), TINO_VA_GET(list), &list2, tino_va_str(list2), tino_va_get(list2)));
  tino_va_end(list2);
  return c;
}

static TINO_PRINT_CTX
tino_printO(TINO_PRINT_CTX c, const char *format, ...)
{
  tino_va_list	list;

  tino_va_start(list, format);
  xDP(("(%p '%s') %p:'%s'%p", c, format, &list, tino_va_str(list), tino_va_get(list)));
  c	= tino_vprintO(c, &list);
  xDP(("() %p %p:'%s'%p", c, &list, tino_va_str(list), tino_va_get(list)));
  tino_va_end(list);
  return c;
}

#endif
