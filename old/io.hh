/* $Header$
 *
 * Upcomming:
 * Generic IO layer.
 *
 * $Log$
 * Revision 1.1  2005-12-04 15:26:04  tino
 * added (incomplete)
 *
 */

#ifndef tino_INC_io_hh
#define tino_INC_io_hh

#include <iostream>

template <class T>
class tino_io_basic : public std::basic_iostream<T>
  {
  private:
    int		flag_stdin;

  public:
    void	def(const char *);

    tino_io_basic() : basic_iostream<T>() { def(NULL); }
    tino_io_basic(const char *s) : basic_iostream<T>() { def(s); }
#if 0
    ~tino_io_basic();
#endif

    int		is_stdin(void) const { return flag_stdin; }
#if 0
    operator const char * const();
#endif
  };

typedef tino_io_basic<unsigned char> tino_io;

#endif
