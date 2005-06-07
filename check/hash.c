/* $Header$
 *
 * Evaluation of hash functions
 *
 * $Log$
 * Revision 1.1  2005-06-07 20:35:20  tino
 * added hash collision test
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/times.h>

#define FNS	512
#define	GENS	1024

static char	*buf;
static size_t	blen;

static void
init(int j)
{
  int	i;

  switch (j>>8)
    {
      case 0:
        for (i=blen; --i>=0; buf[i]=j);
        break;

      case 1:
        for (i=blen; --i>=0; buf[i]=j+i);
        break;

      case 2:
        for (i=blen; --i>=0; buf[i]=j-i);
        break;

      case 3:
        for (i=blen; --i>=0; buf[i]=j^i);
        break;
    }
}

#if 0
/* Schlecht
 */
static long
hash1(int n, const char *p, size_t len)
{
  unsigned long	sum;
  int	i;

  if (!n)
    return ((int)p[0]<<8)|p[len-1];
  sum	= 0;
  for (i=len; --i>=0; )
    {
      sum	+= n;
      sum	^= p[i];
    }
  return sum;
}
#endif

#define	NULLFN1(X,LEN) ((((int)(unsigned char)X[0])<<8)|(unsigned char)(X[(LEN)-1]))

static long
hash1(int n, const char *p, size_t len)
{
  unsigned long	long	sum1, sum2;
  int			i;
  unsigned long		nn;

  if (!n)
    return NULLFN1(p,len);
  nn	= n*n;
  sum1	= 0;
  sum2	= 0;
  i	= len;
  if (i&1)
    sum2	= p[--i];
  while (i>0)
    {
      sum1	*= nn;
      sum1	+= p[--i];
      sum2	*= nn;
      sum2	+= p[--i];
    }
  return sum1*n+sum2;
}

static long
hash2(int n, const char *p, size_t len)
{
  unsigned long		sum;
  int			i;

  if (!n)
    return NULLFN1(p,len);
  sum	= 0;
  for (i=len; --i>=0; )
    {
      sum	*= n;
      sum	+= p[i];
    }
  return sum;
}

static long
hash3(int n, const char *p, size_t len)
{
  unsigned short	sum;
  int			i;

  if (!n)
    return NULLFN1(p,len);
  sum	= 0;
  for (i=len; --i>=0; )
    {
      sum	*= n;
      sum	+= p[i];
    }
  return sum;
}

static const char *
delta(clock_t t)
{
  clock_t	n;
  static char	buf[64];

  n	= sysconf(_SC_CLK_TCK);
  snprintf(buf, sizeof buf, "%ld.%03ld", t/n, (t%n)*1000/n);
  return buf;
}

static void
collision(int v, long *p, int n, long *cnt)
{
  int	c, i, cc;
  long	l;

  i	= n;
  cc	= 0;
  for (c=0; --i>=0; )
    {
      int	j, u;

      l	= p[i];
      u	= 0;
      for (j=i; --j>=0; )
	if (p[j]==l)
	  {
	    p[j]	= p[--i];
	    u++;
	  }
      if (u)
	{
	  cc++;
	  c	+= u+1;
	}
    }
  if (c)
    {
      printf(" %d:%d/%d", v, c, cc);
      cnt[0]++;
      cnt[1]	+= c;
      cnt[2]	+= cc;
    }
}

static long	res[GENS>FNS ? GENS : FNS];

static void
run(long (*fn)(int, const char *, size_t), int c)
{
  struct tms	a, b;
  int		i, j;
  long		cnt[2][3];

  memset(cnt, 0, sizeof cnt);


  for (i=256; --i>=0; buf[i]=~i);
  times(&a);
  for (j=102400; --j>=0; )
    fn(j, buf, blen);
  times(&b);
  printf("\n%d: %s\n", c, delta(b.tms_utime-a.tms_utime));

  printf("\ncollision by value:\n");
  for (i=GENS; --i>=0; )
    {
      init(i);
      for (j=FNS; --j>=0; )
        res[j]	= fn(j, buf, blen);
      collision(i, res, FNS, cnt[0]);
    }
  printf("\n\ncollisions by function:\n");

  for (j=FNS; --j>=0; )
    {
      for (i=GENS; --i>=0; )
        {
          init(i);
          res[i]	= fn(j, buf, blen);
        }
      collision(j, res, GENS, cnt[1]);
    }
  printf("\n\n");
  for (i=0; i<2; i++)
    printf("%d total: %ld/%ld/%ld\n", c, cnt[i][0], cnt[i][1], cnt[i][2]);
}

int
main(int argc, char **argv)
{
  blen	= argc>1 ? atoi(argv[1]) : 256;
  buf	= malloc(blen);

  run(hash1, 1);
  run(hash2, 2);
#if 0
  run(hash3, 3);
#endif
  return 0;
}
