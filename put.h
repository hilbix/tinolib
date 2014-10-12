/* Simple putters
 *
 * Putters are stubs to do output with tino_io_put().
 * Do not forget to call tino_io_flush() afterwards
 * as it's buffered!
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_put_h
#define tino_INC_put_h

#include "io.h"

#if 0
#include <string.h>
#endif

static void
tino_put_s(int io, const char *s)
{
  while (*s)
    tino_io_put(io, *s++);
}

static void
tino_put_oct_digit(int io, unsigned char x)
{
  tino_io_put(io, '0'+(x&7));
}

static void
tino_put_dec_digit(int io, unsigned char x)
{
  tino_io_put(io, '0'+(x%10));
}

static void
tino_put_hex_digit(int io, unsigned char x)
{
  tino_io_put(io, "0123456789abcdef"[x&15]);
}

/* This is uppercase HEX and BASE85 according to RFC1924 (it's usable, so why not?)
 */
static void
tino_put_base_l(int io, int minsize, unsigned long long u, int base)
{
  int	i;

  if (base<2)
    base	= 2;
  if (base>85)
    base	= 85;
  if (--minsize>0 || u>=base)
    tino_put_base_l(io, minsize, u/base, base);
  i	= u%base;
  if ((i+='0')>'9')
    if ((i+='A'-'9'-1)>'Z')
      if ((i+='a'-'Z'-1)>'z')
	if ((i+='!'-'z'-1)>='\"')
	  if (++i>='\'')
	    if (++i>=',')
	      if (++i>='.')
		if ((i+=';'-'.')>='A')
		  if ((i+='^'-'A')>='a')
		    i+='{'-'a';
  tino_io_put(io, i);
}

static void
tino_put_oct(int io, int minsize, unsigned u)
{
  if (minsize<=0)
    minsize	= 1;
  minsize	*= 3;
  while (u>>minsize)
    minsize	+= 3;
  while ((minsize-=3)>=0)
    tino_put_oct_digit(io, (unsigned char)(u>>minsize));
}

static void
tino_put_oct_l(int io, int minsize, unsigned long long u)
{
  if (minsize<=0)
    minsize	= 1;
  minsize	*= 3;
  while (u>>minsize)
    minsize	+= 3;
  while ((minsize-=3)>=0)
    tino_put_oct_digit(io, (unsigned char)(u>>minsize));
}

static void
tino_put_hex(int io, int minsize, unsigned u)
{
  if (minsize<=0)
    minsize	= 1;
  minsize	*= 4;
  while (u>>minsize)
    minsize	+= 4;
  while ((minsize-=4)>=0)
    tino_put_hex_digit(io, (unsigned char)(u>>minsize));
}

static void
tino_put_hex_l(int io, int minsize, unsigned long long u)
{
  if (minsize<=0)
    minsize	= 1;
  minsize	*= 4;
  while (u>>minsize)
    minsize	+= 4;
  while ((minsize-=4)>=0)
    tino_put_hex_digit(io, (unsigned char)(u>>minsize));
}

static void
tino_put_dec(int io, int minsize, unsigned u)
{
  if (--minsize>0 || u>=10)
    tino_put_dec(io, minsize, u/10);
  tino_put_dec_digit(io, u%10);
}

static void
tino_put_dec_l(int io, int minsize, unsigned long long u)
{
  if (--minsize>0 || u>=10)
    tino_put_dec_l(io, minsize, u/10);
  tino_put_dec_digit(io, u%10);
}

static void
tino_put_ansi_c(int io, unsigned char c, const char *esc)
{
  if (c<32 || c>=127 || strchr(esc ? esc : " '", c))
    {
      tino_io_put(io, '\\');
      tino_io_put(io, 'x');
      tino_put_hex(io, 2, c);
    }
  else
    {
      if (c=='\\')
        tino_io_put(io, c);
      tino_io_put(io, c);
    }
}

/** Escape string using ANSI sequence (aka. \-escape).
 *
 * Afterwards the string is properly \-escaped.
 * It does no more contain the characters given in esc.
 *
 * By default, this is the space and a single quote: '
 * So you can use the default for bash $'ANSI-SEQUENCE'
 * and need not to worry about 'read -r x y'
 */
static void
tino_put_ansi_buf(int io, const void *ptr, size_t len, const char *esc)
{
  const unsigned char	*s=ptr;

  for (; len--; s++)
    tino_put_ansi_c(io, *s, esc);
}

static void
tino_put_ansi(int io, const char *s, const char *esc)
{
  tino_put_ansi_buf(io, s, strlen(s), esc);
}

static void
tino_put_ansi_start(int io)
{
  tino_io_put(io, '$');
  tino_io_put(io, '\'');
}

static void
tino_put_ansi_end(int io)
{
  tino_io_put(io, '\'');
}

/* Put shell usable string, either single quoted string or ANSI escape
 * sequence.
 *
 * NULL is output as single -
 *
 * Note that spaces are escaped, too, for more easy 'read -r a b c'
 * usage
 */
static void
tino_put_ansi_if(int io, const char *s)
{
  const char *t;

  if (!s)
    {
      tino_io_put(io, '-');
      return;
    }
  for (t=s; *t; t++)
    if (*t<=32 || *t>=127 || *t=='\'')
      {
	tino_put_ansi_start(io);
	tino_put_ansi(io, s, NULL);
	tino_put_ansi_end(io);
	return;
      }
  tino_io_put(io, '\'');
  tino_put_s(io, s);
  tino_io_put(io, '\'');
}

#endif
