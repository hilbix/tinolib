/* $Header$
 *
 * Simple coder / decoders.
 * Sometimes perhaps replace this by a complete "real" codec layer.
 *
 * $Log$
 * Revision 1.1  2004-04-08 21:39:00  tino
 * New HEX input functions
 *
 */

#ifndef tino_INC_codec_h
#define tino_INC_codec_h

static __inline__ int
tino_dec_hex_digit(char c)
{
  switch (c)
    {
    case '0':	return 0;
    case '1':	return 1;
    case '2':	return 2;
    case '3':	return 3;
    case '4':	return 4;
    case '5':	return 5;
    case '6':	return 6;
    case '7':	return 7;
    case '8':	return 8;
    case '9':	return 9;
    case 'a':	return 10;
    case 'b':	return 11;
    case 'c':	return 12;
    case 'd':	return 13;
    case 'e':	return 14;
    case 'f':	return 15;
    }
  return -1;
}

/* Decode at max len bytes of a hex string into a buffer
 * Return number decoded, else error.
 * if n*2==strlen(hex) decoding is complete.
 */
static int
tino_dec_hex(void *p, int len, const char *hex)
{
  int	i;

  for (i=0; i<len; i++)
    {
      int	c;

      c	= tino_dec_hex_digit(*hex++)<<4;
      c	|= tino_dec_hex_digit(*hex++);
      if (c<0)
	return i;
      *((char *)p)++	= c;
    }
  return i;
}

#endif
