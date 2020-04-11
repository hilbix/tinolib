/* Path utilities
 */

static const char *                                                                             
basename(const char *s)
{
  const char    *p;

  FATAL(!s);
  p     = strrchr(s, '/');
  return p ? p+1 : s;
}

