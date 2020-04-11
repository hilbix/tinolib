/* MiniLib VA_ARGS
 *
 * Because I needed some matching type for _Generic
 */

#include <stdarg.h>

typedef struct VA_LIST
  {
    va_list	list;
  } VA_LIST[1];

#define	VA_START(List,s)	do { va_start(List[0].list, s); } while (0)
#define	VA_END(List)		do { va_end(List[0].list);      } while (0)

