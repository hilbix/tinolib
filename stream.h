/* NOT READY YET *
 * UNIT TEST FAILS *
 *
 * Stream processing
 *
 * TINO_STREAM s = tino_stream_new();
 * tino_stream_src_file(s, "/tmp/whatever");
 * tino_stream_std_gunzip(s);
 * tino_stream_read(s, &buf, sizeof buf);
 *
 * tino_stream_connect(s, modnr1, socknr1, modnr2, socknr2);
 * tino_stream_disconnect(s, modnr, socknr);
 * tino_stream_max(s, modnr1);
 *
 * Streams are allocated generalized data transformation flows.  Each
 * stream has sockets and a module associated which processes the
 * data.  Sockets with even numbers are inputs (data to the module,
 * you write() to it) and sockets with odd numbers are outputs (data
 * from the module, you read() from it).  Sockets can be connected to
 * other sockets of other streams to form arbitrary complex data
 * processing networks.
 *
 * Error processing is done implicitely.  If an error condition cannot
 * be handled internally, the program terminates.
 *
 * Sockets can be closed (unconnected), open (connected and accepting
 * data) or blocked (connected but cannot accept data).  The latter is
 * to avoid deadlock or recoursion overflow situations (like a stream
 * feeding itself).
 *
 * The default module is the "nul" module which just discards data on
 * inputs and does EOF on outputs.  It is fully re-entrant, which
 * means, it never blocks, even if connected to itself.
 *
 * Sink modules only have an in(0)-socket which consumes all data.
 * Src modules only have an out(1)-socket and provides data.
 *
 * Note that you usually start with either a src or sink module and
 * add the other modules to it.  If you add a module and it fits
 * in the puzzle, it is automatically connected according to it's
 * preference.  If this is impossible, you must do the connections
 * yourself. module has a preference,
 * which socket must be connected next.  However you can define the
 * next socket which shall be used to connect and you can change
 * connections any time.
 * 
 * Std modules are code snippets, which transform the data while it
 * is transferred.  These snippets can keep status while transforming.
 * They use in(0)- and an out(1)-sockets.
 *
 * Bi modules use two streams together to do some more complex
 * transformations, like protocol validation.  There is in(0)- and
 * out(1)-sockets and corrsponding down(2)- and up(3)-sockets.
 *
 * Hub modules have n sockets (n=0 to unlimited) which can be incoming
 * (even numbers) or outgoing (odd numbers).  Effectively, everything
 * is a (specialized) hub.
 *
 * Streams can be interconnected in a network fashion.  The structure
 * can be saved to a file and read in again.  The save can be with
 * context information and without.  Saving needs module support.
 *
 * You can create your own streams module by specializing any other
 * streams module.  There are standard calls to put and get
 * various settings.
 *
 * Copyright (C)2009-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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
