/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-02-14 20:27:09  tino
 * Initial add
 *
 */

/*IMP*/

static void *
tino_alloc(size_t len)
{
  void	*p;

  if ((p=malloc(len))==0)
    ex("malloc");
  return p;
}

static char *
tino_stralloc(const char *s)
{
  char	*p;

  if ((p=strdup(s))==0)
    ex("strdup");
  return p;
}
