/* $Header$
 *
 * $Log$
 * Revision 1.1  2004-08-22 23:21:01  tino
 * sync to cvs
 *
 */

static const tino_ob *ob_str=tino_ob_register(TINO_OB_STR);

typedef struct tino_str tino_str;
struct tino_str
  {
    struct tino_ob	*ob;
    size_t		len, max;
    char		*s;
  };

static tino_str *
tino_str_new(void)
{
  tino_str	*p;

  p		= tino_ob_new(
  p		= tino_alloc(sizeof *p);
  p->ob.type	= TINO_OB_STR;
  p->len	= 0;
  p->max	= 0;
  p->s		= "";
  return p;
}

static void
tino_str_free(tino_str *p)
{
  if (p->s)
    free(p->s);
  p->len	= 0;
  p->max	= 0;
  p->s		= 0;
  free(p);
}

static tino_str *
tino_str_extend(tino_str *p, size_t len)
{
  if (!p)
    p	= tino_str_new();
  if (p->max<len)
    {
      p->s	= tino_realloc(p->s, len);
      p->max	= len;
    }
  return p;
}

static tino_str *
tino_str_clear(tino_str *p)
{
  p		= tino_str_extend(p, 1);
  p->len	= 0;
  p->s[0]	= 0;
  return p;
}

static tino_str *
tino_str_append_p(tino_str *p, const void *s, size_t len)
{
  if (!p)
    p	= tino_str_new();
  if (p->len+len>=p->max)
    tino_str_extend(p, p->len+len+1);
  memcpy(p->s+p->len, s, len);
  p->len	+= len;
  p->s[p->len]	= 0;
  return p;
}

static tino_str *
tino_str_append_s(tino_str *p, const char *s)
{
  return tino_str_append_p(p, s, strlen(s));
}

static tino_str *
tino_str_append(tino_str *p, tino_str *s)
{
  return tino_str_append_p(p, s->s, s->len);
}

static tino_str *
tino_str_set_s(tino_str *p, const char *s)
{
  return tino_str_append_s(tino_str_clear(p), s);
}

static tino_str *
tino_str_explode_s(tino_str *p,
		   const char *sep,
		   const char * const *a, int len)
{
  int	flag;

  if (!p)
    p	= tino_str_new();
  for (flag=0; len; flag=1, len--, a++)
    if (*a)
      {
	if (flag)
	  tino_str_append_s(p, sep);
	tino_str_append_s(p, *a);
      }
    else if (len<0)
      break;
  return p;
}

static tino_str *
tino_str_append_pathchar(tino_str *p)
{
  if (p && p->len && p->s[p->len-1]!='/')
    tino_str_append_p(p, "/", 1);
  return p;
}
