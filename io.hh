/* UNIT TEST FAILS *
 *
 * Upcomming:
 * Generic C++ IO layer.
 *
 * Copyright (C)2004-2014 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
