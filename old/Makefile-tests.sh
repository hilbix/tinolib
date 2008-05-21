#!/bin/bash
# $Header$
#
# This is a general test script for commandline programs.
# It operates on the file "Tests"
#
# Copyright (C)2008 Valentin Hilbig <webmaster@scylla-charybdis.com>
#
# Do not expect to be able to run it at your side.
# This is undocumented, as tests are considered to be done by me only.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA.
#
# $Log$
# Revision 1.1  2008-05-21 17:58:13  tino
# make test
#

set -e

oops()
{
echo "$0 OOPS: line $lineno '$cmd $args': $*" >&2
false
}

undir()
{
rmdir "$1" || oops "
Running:  $last
Nonempty: directory ${2:-$1} ($1)
Content:  `ls -p "$1"`"
}

cleanup()
{
[ -n "$TMPDIR" ] || return 0
case "$TMPDIR" in
*/*)	oops "TMPDIR violation";;
tmp.*)	;;
*)	oops "TMPDIR violation";;
esac
[ -z "$1" ] || undir "$TMPDIR" "$1"
rm -rf "$TMPDIR"
TMPDIR=""
}

arg()
{
[ -n "$TMPDIR" ] || oops "sequence error"
case "$1" in
*..*)	oops "ARG violation: $1";;
esac
}

rnd()
{
md5chk -dq "random $RAND $*"
}

start()
{
[ -z "$TMPDIR" ] || return 0
TMPDIR="`mktemp -d tmp.XXXXXXXXXX`"
let ++testcount
}

# Create a working dir
cmd-dir()
{
start
for a
do
	arg "$a"
	mkdir "$TMPDIR/$a"
done
}

# Create a working file with some random content
cmd-file()
{
start
arg "$2"
rnd "$1" > "$TMPDIR/$2"
}

# Run command in TMPDIR
cmd-run()
{
start
last="$*"
[ -d "$TMPDIR" ]
set +e
out="`{ PATH="..:$PATH"; cd "$TMPDIR" && "$@"; } 2>&1`"
res="$?"
set -e
}

# Check results
cmd-RUN()
{
[ ".$res" = ".$1" ] || oops "
Running:  $last
Expected: $1
Result:   $res"
shift
[ ".$out" = ".$*" ] || oops "
Running:  $last
Expected: $*
Output:   $out"
}

# Check presence of Dir
cmd-DIR()
{
for a
do
	arg "$a"
	[ -d "$TMPDIR/$a" ] || oops "
Running:  $last
Missing:  directory $a"

	undir "$TMPDIR/$a" "$a"
done
}

# Check presence of File
cmd-FILE()
{
arg "$2"

[ -f "$TMPDIR/$2" ] || oops "
Running:  $last
Missing:  file $a"

rnd "$1" | cmp -s - "$TMPDIR/$2" || oops "
Running:  $last
Mismatch: file $a
Expected: `rnd "$1"`
Got:      `cat "$TMPDIR/$2"`"

rm -f "$TMPDIR/$2"
}

run()
{
TMPDIR=
RAND="`date` `uptime` $$ $0"

trap 'cleanup' 0

let testcount=0 ||:
let lineno=0 ||:
while read -r cmd args
do
	let ++lineno
	: $cmd $args
	case "$cmd" in
	\#*)				continue;;
	'')				cleanup "run-directory";;
	dir|file|run|DIR|FILE|RUN)	"cmd-$cmd" $args <&3;;
	*)				oops "unknown command: $cmd $args";;
	esac
done 3>&0 <"$1"

echo "Successful $testcount tests on $lineno lines"
}

echo
( run "$@" )
ret="$?"
[ 0 = "$ret" ] || echo "Test failed, code $ret" >&2
exit $ret
