#include <stdio.h>
#include <errno.h>

#define	PERROR(X)	errno=X; fprintf(stderr, "%3d: ",X); perror(#X)

int
main(void)
{
  int i;

  for (i=0; ++i<200; )
    {
      PERROR(i);
    }
  return 0;
}

