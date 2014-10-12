/* Generic Iterator
 *
 * This is a gerneric iterator implementation over containers.
 *
 * An iterator is something which runs over containers which are
 * iterable, like trees or lists.  Iterators only return the pointers
 * to the data, never to key values (the underlying implementation
 * often is not capable of supplying key values).  If you need key
 * values, you must wrap this into the data pointer.
 *
 * Iterable containers have following methods:
 *
 * tino_*_iterO() gets an iterator.  There container itself is an iter
 * tino_*_resetO() resets the iterator
 * tino_*_nextO() gets the next entry (first after reset)
 * tino_*_endO() frees the iterator (on the container this is reset)
 *
 * tino_*_initO() initializes the container (memset(o, 0, sizeof *o))
 * tino_*_freeO() kills the container (frees only internal data)
 *
 * tino_*_get*() gets the pointer to the data, NULL if missing.
 * tino_*_set*() sets a data of entry, returns true on error (exists).
 * tino_*_add*() adds new entry, returns pointer to data pointer.
 * tino_*_del*() deletes an entry, returns false if not present.
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

#ifndef tino_INC_iter_h
#define tino_INC_iter_h

#endif
