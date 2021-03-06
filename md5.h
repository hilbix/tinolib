/* MD5 routine in case you want not to include bloat.
 *
 * I do not copyright this, as this was stolen somewhere and hacked to
 * fit here in.  I cannot tell where I found it.  I cannot tell how it
 * was.  I just hacked it to fit into tinolib.  The original was not
 * copyrighed, too.
 *
 * Old comments left intact as much as possible even that this
 * comments are wrong now.
 */
#ifndef tino_INC_md5_h
#define tino_INC_md5_h

#include "type.h"

#include <stdio.h>
#include <string.h>

#ifndef TINO_NEED_MD5_COMPAT
#define	tino_md5_ctx	MD5_CTX
#define	tino_md5_init	MD5Init
#define	tino_md5_update	MD5Update
#define	tino_md5_final(A,B)	MD5Final(B,A)
#endif

struct tino_md5_context
  {
    tino_u32_t		buf[4];
    tino_u32_t		bits[2];
    unsigned char	in[64];
  };

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct tino_md5_context tino_md5_ctx;

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.	This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

/* Brutally hacked by John Walker back from ANSI C to K&R (no
   prototypes) to maintain the tradition that Netfone will compile
   with Sun's original "cc". */

#ifdef sgi
#define HIGHFIRST
#endif

#ifdef sun
#define HIGHFIRST
#endif

#ifndef HIGHFIRST
#define tino_md5_byterev(buf, len)	/* Nothing */
#else
/*
 * Note: this code is harmless on little-endian machines.
 */
static void
tino_md5_byterev(unsigned char *buf, unsigned longs)
{
    tino_u32_t t;
    do {
	t = (tino_u32_t) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
	    ((unsigned) buf[1] << 8 | buf[0]);
	*(tino_u32_t *) buf = t;
	buf += 4;

    } while (--longs);
}
#endif

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void
tino_md5_init(tino_md5_ctx *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define tino_F1(x, y, z) (z ^ (x & (y ^ z)))
#define tino_F2(x, y, z) tino_F1(z, x, y)
#define tino_F3(x, y, z) (x ^ y ^ z)
#define tino_F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define tino_MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void
tino_md5_transform(tino_u32_t buf[4], tino_u32_t in[16])
{
    register tino_u32_t a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    tino_MD5STEP(tino_F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    tino_MD5STEP(tino_F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    tino_MD5STEP(tino_F1, c, d, a, b, in[2] + 0x242070db, 17);
    tino_MD5STEP(tino_F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    tino_MD5STEP(tino_F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    tino_MD5STEP(tino_F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    tino_MD5STEP(tino_F1, c, d, a, b, in[6] + 0xa8304613, 17);
    tino_MD5STEP(tino_F1, b, c, d, a, in[7] + 0xfd469501, 22);
    tino_MD5STEP(tino_F1, a, b, c, d, in[8] + 0x698098d8, 7);
    tino_MD5STEP(tino_F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    tino_MD5STEP(tino_F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    tino_MD5STEP(tino_F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    tino_MD5STEP(tino_F1, a, b, c, d, in[12] + 0x6b901122, 7);
    tino_MD5STEP(tino_F1, d, a, b, c, in[13] + 0xfd987193, 12);
    tino_MD5STEP(tino_F1, c, d, a, b, in[14] + 0xa679438e, 17);
    tino_MD5STEP(tino_F1, b, c, d, a, in[15] + 0x49b40821, 22);

    tino_MD5STEP(tino_F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    tino_MD5STEP(tino_F2, d, a, b, c, in[6] + 0xc040b340, 9);
    tino_MD5STEP(tino_F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    tino_MD5STEP(tino_F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    tino_MD5STEP(tino_F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    tino_MD5STEP(tino_F2, d, a, b, c, in[10] + 0x02441453, 9);
    tino_MD5STEP(tino_F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    tino_MD5STEP(tino_F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    tino_MD5STEP(tino_F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    tino_MD5STEP(tino_F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    tino_MD5STEP(tino_F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    tino_MD5STEP(tino_F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    tino_MD5STEP(tino_F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    tino_MD5STEP(tino_F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    tino_MD5STEP(tino_F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    tino_MD5STEP(tino_F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    tino_MD5STEP(tino_F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    tino_MD5STEP(tino_F3, d, a, b, c, in[8] + 0x8771f681, 11);
    tino_MD5STEP(tino_F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    tino_MD5STEP(tino_F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    tino_MD5STEP(tino_F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    tino_MD5STEP(tino_F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    tino_MD5STEP(tino_F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    tino_MD5STEP(tino_F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    tino_MD5STEP(tino_F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    tino_MD5STEP(tino_F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    tino_MD5STEP(tino_F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    tino_MD5STEP(tino_F3, b, c, d, a, in[6] + 0x04881d05, 23);
    tino_MD5STEP(tino_F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    tino_MD5STEP(tino_F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    tino_MD5STEP(tino_F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    tino_MD5STEP(tino_F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    tino_MD5STEP(tino_F4, a, b, c, d, in[0] + 0xf4292244, 6);
    tino_MD5STEP(tino_F4, d, a, b, c, in[7] + 0x432aff97, 10);
    tino_MD5STEP(tino_F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    tino_MD5STEP(tino_F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    tino_MD5STEP(tino_F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    tino_MD5STEP(tino_F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    tino_MD5STEP(tino_F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    tino_MD5STEP(tino_F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    tino_MD5STEP(tino_F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    tino_MD5STEP(tino_F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    tino_MD5STEP(tino_F4, c, d, a, b, in[6] + 0xa3014314, 15);
    tino_MD5STEP(tino_F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    tino_MD5STEP(tino_F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    tino_MD5STEP(tino_F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    tino_MD5STEP(tino_F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    tino_MD5STEP(tino_F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void
tino_md5_update(tino_md5_ctx *ctx, const void *_buf, size_t len)
{
    tino_u32_t		t;
    const unsigned char	*buf = (const unsigned char *)_buf;

    /* Update bitcount */

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((tino_u32_t) len << 3)) < t)
	ctx->bits[1]++; 	/* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */

    if (t) {
	unsigned char *p = (unsigned char *) ctx->in + t;

	t = 64 - t;
	if (len < t) {
	    memcpy(p, buf, len);
	    return;
	}
	memcpy(p, buf, t);
	tino_md5_byterev(ctx->in, 16);
	tino_md5_transform(ctx->buf, (tino_u32_t *) ctx->in);
	buf += t;
	len -= t;
    }
    /* Process data in 64-byte chunks */

    while (len >= 64) {
	memcpy(ctx->in, buf, 64);
	tino_md5_byterev(ctx->in, 16);
	tino_md5_transform(ctx->buf, (tino_u32_t *) ctx->in);
	buf += 64;
	len -= 64;
    }

    /* Handle any remaining bytes of data. */

    memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void
tino_md5_final(tino_md5_ctx *ctx, unsigned char digest[16])
{
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
	/* Two lots of padding:  Pad the first block to 64 bytes */
	memset(p, 0, count);
	tino_md5_byterev(ctx->in, 16);
	tino_md5_transform(ctx->buf, (tino_u32_t *) ctx->in);

	/* Now fill the next block with 56 bytes */
	memset(ctx->in, 0, 56);
    } else {
	/* Pad block to 56 bytes */
	memset(p, 0, count - 8);
    }
    tino_md5_byterev(ctx->in, 14);

    /* Append length in bits and transform */
    ((tino_u32_t *) ctx->in)[14] = ctx->bits[0];
    ((tino_u32_t *) ctx->in)[15] = ctx->bits[1];

    tino_md5_transform(ctx->buf, (tino_u32_t *) ctx->in);
    tino_md5_byterev((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(*ctx));	/* In case it's sensitive */
}

#undef	tino_md5_byterev
#undef	tino_F1
#undef	tino_F2
#undef	tino_F3
#undef	tino_F4
#undef	tino_MD5STEP

static void
tino_md5_bin(const void *ptr, size_t len, unsigned char digest[16])
{
  tino_md5_ctx	ctx;

  tino_md5_init(&ctx);
  tino_md5_update(&ctx, ptr, len);
  tino_md5_final(&ctx, digest);
}

static void
tino_md5_hex(tino_md5_ctx *ctx, unsigned char out[33])
{
  unsigned char	digest[16];
  size_t	i;

  tino_md5_final(ctx, digest);
  for (i=0; i<sizeof digest; i++)
    sprintf((char *)(out+i+i), "%02x", digest[i]);
}

static void
tino_md5_buf(const void *ptr, size_t len, unsigned char out[33])
{
  tino_md5_ctx	ctx;

  tino_md5_init(&ctx);
  tino_md5_update(&ctx, ptr, len);
  tino_md5_hex(&ctx, out);
}

static void
tino_md5_str(const char *s, unsigned char out[33])
{
  tino_md5_buf(s, strlen(s), out);
}

#ifdef TINO_TEST_MAIN
#undef TINO_TEST_MAIN

#include <stdio.h>

int
main(int argc, char **argv)
{
  tino_md5_ctx	ctx;
  unsigned char	digest[33];
  int		i;

  tino_md5_init(&ctx);
  for (i=1; i<argc; i++)
    {
      int	got;
      FILE	*fd;
      char	buf[BUFSIZ*10];

      if ((fd=fopen(argv[i], "rb"))==NULL)
        {
	  perror(argv[i]);
	  continue;
        }
      while ((got=fread(buf, 1, sizeof buf, fd))>0)
	tino_md5_update(&ctx, buf, got);
      if (fclose(fd))
        {
	  perror(argv[i]);
	  continue;
	}
    }
  tino_md5_hex(&ctx, digest);
  printf("%s\n", digest);
  return 0;
}
#endif
#endif
