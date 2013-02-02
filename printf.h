/* $Header$
 *
 * UNIT TEST FAILS *
 * This currently is only an idea
 *
 * My own version of printf.
 *
 * 1) vprintf(str, args) is written as printf("%v", TINO_VA_LIST)
 * 2) There are no sideffects on the TINO_VA_LIST
 * 3) You can skip and re-order args.  (There can be no holes!)
 *
 * Not supported:
 *
 * - No specials: # ' ' +
 * - No locales: ' I
 * - No floating point: L e E f F g G a A 
 * - No unusuals: q j z t C S m
 * - No char: c
 * - No wchar_t: lc and ls
 * - %v does not support size
 *
 * Supported:
 *
 * - Size: [[-][0]width][.precision]
 * - Length: hh h l ll
 * - Integer: d i
 * - Unsigned: o u x X
 * - String: s
 * - void *: p
 * - offset: n
 * - percent: %
 * - varparm: *
 *
 * Extensions:
 *
 * - v: recursive TINO_VA_LIST
 * - _#: argument number for reordering
 * - !: argument is ignored (not rendered)
 * - [filter]: special rendering filter
 * - {function}: special rendering function
 * - (expr): special rendering expression
 *
 * Copyright (C)2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
 *
 * $Log$
 * Revision 1.1  2008-05-27 21:24:48  tino
 * added
 *
 */

/* _ extension explained:
 *
 * _ must come after size.
 *
 * _0 is the same as _ and denotes the same argument:
 * printf("Dec %d Octal %_o Hex %_x\n", 42);
 *
 * _* is not supported for apparently reasons.  Else it works straight
 * forward, a * in the size is fetched before the argument as usual
 * and is not counted.  Note that you must give the stars on the first
 * definition, later you can leave them away.  Example:
 * printf("Short %-*.*s: Long %-*s\n", 5, 4, "thisisalongtext");
 *
 * There may not be any holes, following is an error:
 * printf("%_3d\n", "hello", 5.5, 42)
 * Note that you can fix that (as soon f is supported) with:
 * printf("%!s%!f%d\n", "hello", 5.5, 42)
 * Also following would be ok then:
 * printf("%_3d %_2f %_1s\n", "hello", 5.5, 42)
 */

/* %v extension explained:
 *
 * A TINO_VA_LIST is fetched like any other object (this is a pointer).
 *
 * It then is rendered recursively (as the formatting is in the TINO_VA_LIST).
 *
 * A size is not yet supported.
 *
 * Indexing is not yet supported.  Indexing will look like: %V5d
 */

/* {function} [filter] (expr) explained:
 *
 * There are filters (which modify the results) and functions (which
 * provide the argument) and expressions (which provide options).
 *
 * Following expressions exist (expr are automatically converted as needed):
 * NUMBER: The given NUMBER.  0xHEX, 0oOCTAL, 0bBINARY
 * 'string' or "string" or /string/: The given string
 * (expr): evaluates the expression
 * prefixEXPR: Call a prefix function with argument EXPR
 * function(EXPR,...): Call a function with arguments
 * function expr ...: Call function (alternative form)
 *
 * Following functions exist:
 * $ENV: Accesse the ENV variable ENV.  $(expr) is ok.
 * @EXPR: Makes a string of the expression (like @05d). @(expr) is ok.
 * &NR: Character NR (later Unicode is supported)
 * sub(expr,off) and sub(expr,off,len): Sub strings
 * date(expr,format): Format date as in strftime
 * parse(expr,format): Parse date as in strptime
 *
 * Filters are functions which get the result as the first argument,
 * given arguments extend the function.  Multiple filters are applied
 * in sequence right to left.  Filters can be nested as in
 * %{$(@[sub(3,1)]05_3d)}s
 * which is the same as
 * %[$][sub 3 1]05_3d
 * which is the same as
 * %{$(sub(@05_3d,3,1))}s
 * For me the 2nd form is most readable (this is why filters exist).
 *
 * You can register your own functions.  The API will be presented
 * here as soon as it is ready.  Some names are reserved for future:
 *
 * enc(expr,type), dec(expr,type): Encode/Decode base32/base64/mime
 *
 */
