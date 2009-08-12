/* Stolen from lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 * http://burtleburtle.net/bob/c/lookup3.c
 */

#ifndef tino_INC_bob_jenkins_h
#define tino_INC_bob_jenkins_h

#define bob_mix(a,b,c) \
  { \
  a -= c;  a ^= bob_rot(c, 4);  c += b; \
  b -= a;  b ^= bob_rot(a, 6);  a += c; \
  c -= b;  c ^= bob_rot(b, 8);  b += a; \
  a -= c;  a ^= bob_rot(c,16);  c += b; \
  b -= a;  b ^= bob_rot(a,19);  a += c; \
  c -= b;  c ^= bob_rot(b, 4);  b += a; \
  }

#define bob_final(a,b,c) \
  { \
  c ^= b; c -= bob_rot(b,14); \
  a ^= c; a -= bob_rot(c,11); \
  b ^= a; b -= bob_rot(a,25); \
  c ^= b; c -= bob_rot(b,16); \
  a ^= c; a -= bob_rot(c,4);  \
  b ^= a; b -= bob_rot(a,14); \
  c ^= b; c -= bob_rot(b,24); \
  }

#define bob_rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

static uint32_t
bob_hashlittle( const void *key, size_t length, uint32_t initval)
{
  const tino_u8_t *k = (const tino_u8_t *)key;
  tino_u32_t	a,b,c;

  a = b = c = 0xdeadbeef + ((tino_u32_t)length) + initval;

  while (length > 12)
    {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      bob_mix(a,b,c);
      length -= 12;
      k += 12;
    }

  switch (length)
    {
    case 12: c+=((uint32_t)k[11])<<24;
    case 11: c+=((uint32_t)k[10])<<16;
    case 10: c+=((uint32_t)k[9])<<8;
    case 9 : c+=k[8];
    case 8 : b+=((uint32_t)k[7])<<24;
    case 7 : b+=((uint32_t)k[6])<<16;
    case 6 : b+=((uint32_t)k[5])<<8;
    case 5 : b+=k[4];
    case 4 : a+=((uint32_t)k[3])<<24;
    case 3 : a+=((uint32_t)k[2])<<16;
    case 2 : a+=((uint32_t)k[1])<<8;
    case 1 : a+=k[0];
      break;
    case 0 : return c;
    }

  bob_final(a,b,c);
  return c;
}

#endif
