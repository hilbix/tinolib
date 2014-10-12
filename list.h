/* Generic Data List
 *
 * See iter.h for more information
 *
 * This is a gerneric list implementation in that the representation
 * is hidden from the caller.
 *
 * Currently there only is "plist", which means a list storing unique
 * pointers (void *) associated to some other pointer.  (Usually you
 * will have both pointers be the same, as you cannot iterate over the
 * key pointers, only over the value pointers.)
 *
 * Copyright (C)2008-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef tino_INC_list_h
#define tino_INC_list_h

#include "iter.h"

#endif
