/* Symmetric buffer quoting and unquoting
 *
 * Bi-directional buffer string handling with quoting/unquoting.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_buf_quote_h
#define tino_INC_buf_quote_h

#include "buf.h"
#include <ctype.h>

/** C-style escape: str -> "str"
 */
static TINO_BUF *
tino_buf_quoteN(TINO_BUF *buf, const char *s)
{
  const char	*hex = "0123456789abcdef";
  int		c;
  const int	quote='"';
  const int	esc='\\';

  if (!buf)	/* create buffer is none given	*/
    buf	= tino_alloc0O(sizeof *buf);

  /* add a quote	*/
  tino_buf_add_cO(buf, quote);

  /* copy the characters	*/
  while ((c = (unsigned char)*s++)!=0)
    {
      if (c<32 || c=='\\' || c=='"')
        {
          /* need some escapement	*/
          tino_buf_add_cO(buf, esc);
          switch (c)
            {
            default:		/* \xHH	*/
              tino_buf_add_cO(buf, 'x');
              tino_buf_add_cO(buf, hex[(c>>4)&0xf]);
              c	= hex[(c>>0)&0xf];
              break;

            case '\a':	c='a';	break;
            case '\b':	c='b';	break;
            case '\177': c='d';	break;
            case '\33':	c='e';	break;
            case '\f':	c='f';	break;
            case '\n':	c='n';	break;
            case '\r':	c='r';	break;
            case '\t':	c='t';	break;
            case '\v':	c='v';	break;

            case '\\':
            case '"':	break;
            }
        }
      tino_buf_add_cO(buf, c);
    }
  tino_buf_add_cO(buf, quote);
  return buf;
}

/** C-style unquote next argument.
 *
 * This removes quotes, leading and trailing spaces and reads the string with dequoting.
 *
 * If delim is given (>0) then scanning also stops on unquoted delimiter.
 * If delim is missing, this is an error.
 */
static const char *
tino_buf_unquoteN(TINO_BUF *buf, int delim)
{
  char		*ptr, *tmp;
  int		i, o, max;
  const int	quote='"';
  const int	esc='\\';

  max	= tino_buf_get_lenO(buf);
  ptr	= tino_buf_get_s_nonconstO(buf);
  i	= 0;

  /* remove leading spaces	*/
  for (i=0; i<max && isspace(ptr[i]); i++);
  if (i>=max)
    return 0;	/* nothing there	*/

  if (ptr[i]!=quote)
    {
      int	end;

      /* unescaped/unquoted argument	*/
      tmp	= ptr+i;
      for (;; i++)
        if (i >= max)
          {
            /* we hit the end of the road	*/
            end	= i;
            break;
          }
        else if (isspace(ptr[i]) || (unsigned char)ptr[i] == delim)
          {
            /* either hit a space or the delimiter	*/
            end	= i;

            /* remove spaces after this argument
             * and check if delimiter was hit while doing so
             */
            do
              {
                if ((unsigned char)ptr[i] == delim)
                  delim	= 0;
              } while (++i<max && isspace(ptr[i]));
            break;
          }
      if (delim > 0)
        {
          /* check that we have the delimiter here	*/
          if (i >= max || ((unsigned char)ptr[i]) != delim)
            return 0;
          /* remove spaces after delimiter	*/
          while (++i<max && isspace(ptr[i]));
        }
      /* terminate the string, advance buffer and return it	*/
      ptr[end]	= 0;
      tino_buf_advance_nO(buf, i);
      return tmp;
    }

  /* quoted string
   *
   * Copy it into some temporary buffer
   */
  tmp	= tino_allocO(max);
  o	= 0;
  while (++i<max)
    {
      int	c;

      if (ptr[i]==quote)
        {
          /* Hit quote again, so the string ended.
           *
           * remove spaces after quote.
           * Check if we hit the delimiter while doing so.
           */
          while (++i<max && isspace(ptr[i]))
            if (ptr[i] == delim)
              delim	= 0;

          if (delim > 0)
            {
              /* check that we have the delimiter here	*/
              if (i >= max || ((unsigned char)ptr[i]) != delim)
                break;
              /* remove spaces after delimiter	*/
              while (++i<max && isspace(ptr[i]));
            }

          /* advance the buffer	*/
          tino_buf_advance_nO(buf, i);

          /* copy the temporary unquoted string into the buffer	*/
          memcpy(ptr, tmp, o);

          /* We do not need the temporary buffer anymore	*/
          tino_freeO(tmp);

          /* "argument" means, we always have 2 bytes room for the NUL	*/
          ptr[o]	= 0;
          return ptr;
        }

      /* not the quote character	*/
      if ((c=(unsigned char)ptr[i]) == esc)
        {
          /* we got a \\ (escape code)	*/
          if (++i >= max)
            break;	/* cannot end on esc	*/
          switch (ptr[i])
            {
            case 'x':	/* \\xHH	*/
              if ((i+=2) < max)
                {
                  c	= (tino_dec_hex_digitO(ptr[i-1])<<4) | tino_dec_hex_digitO(ptr[i-0]);
                  break;
                }
              /* fallthrough to error	*/
            default:	c= -1;	break;

            case 'a':	c='\a';	break;
            case 'b':	c='\b';	break;
            case 'd':	c=127;	break;
            case 'e':	c='\33'; break;
            case 'f':	c='\f';	break;
            case 'n':	c='\n';	break;
            case 'r':	c='\r';	break;
            case 't':	c='\t';	break;
            case 'v':	c='\v';	break;
            case '\\':	break;
            }
        }
      if (c<0)
        break;	/* error condition	*/
      tmp[o++]	= c;
    }

  /* Some problem occured, free the temporary buffer and return NULL	*/
  tino_freeO(tmp);
  return NULL;
}

/* Trim buffer at the beginning and return first character, -1 if buffer empty	*/
static int
tino_buf_trimO(TINO_BUF *buf)
{
  const char	*ptr;
  int		max, i;

  max	= tino_buf_get_lenO(buf);
  ptr	= tino_buf_getN(buf);

  for (i=0; i<max && isspace(ptr[i]); i++);
  tino_buf_advance_nO(buf, i);
  return i>=max ? -1 : (unsigned char)ptr[i];
}

#endif

