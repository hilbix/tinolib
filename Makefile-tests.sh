#!/bin/bash
#
# This is a general test script for commandline programs.
# It operates on the file "Tests"
#
# Do not expect to be able to run it at your side.
# This is undocumented, as tests are considered to be done by me only.
#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

set -e

oops()
{
echo "$0 OOPS: line $lineno '${args[*]}': $*" >&2
[ -z "$TMPDIR" ] || find "$TMPDIR" -ls
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
out="`{ set +x; { PATH="..:$PATH"; cd "$TMPDIR" && eval "$@"; } 2>&1; }`"
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
cmp="$*"
cmp="${cmp//@_/
}"
cmp="${cmp//@+/ }"
cmp="${cmp//@t/$'\t'}"
cmp="${cmp//@-/@}"
[ ".$out" = ".$cmp" ] || oops "
Running:  $last
Expected: `printf %q "$cmp"`
Output:   `printf %q "$out"`"
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
Missing:  file $2"

rnd "$1" | cmp -s - "$TMPDIR/$2" || oops "
Running:  $last
Mismatch: file $2
Expected: `rnd "$1"`
Got:      `cat "$TMPDIR/$2"`"

rm -f "$TMPDIR/$2"
}

# first compare
cmd-cmp()
{
start
last1="$*"
[ -d "$TMPDIR" ]
set +e
out1="`{ set +x; { PATH="..:$PATH"; cd "$TMPDIR" && eval "$@"; } 2>&1; }`"
res1="$?"
set -e
}

# second compare
cmd-CMP()
{
last2="$*"
[ -d "$TMPDIR" ]
set +e
out2="`{ set +x; { PATH="..:$PATH"; cd "$TMPDIR" && eval "$@"; } 2>&1; }`"
res2="$?"
set -e

[ .00 = ".$res1$res2" -a ".$out1" = ".$out2" ] || oops "
Compare:  $last1
with:     $last2
result 1: $res1
result 2: $res2
output 1: '$out1'
output 2: '$out2'"
}

# need some program(s)
cmd-need()
{
for a
do
	which "$a" >/dev/null || oops "missing installed program: $a"
done
}

run()
{
TMPDIR=
RAND="`date` `uptime` $$ $0"

trap 'cleanup' 0

let testcount=0 ||:
let lineno=0 ||:
while read -ra args
do
	let ++lineno
	: "${args[@]}"
	case "${args[0]}" in
	\#*)					continue;;
	'')					cleanup "run-directory";;
	need|dir|file|run|cmp|DIR|FILE|RUN|CMP)	"cmd-${args[0]}" "${args[@]:1}" <&3;;
	*)					oops "unknown command: ${args[*]}";;
	esac
done 3>&0 <"$1"

echo "Successful $testcount tests on $lineno lines"
}

echo
( run "$@" )
ret="$?"
[ 0 = "$ret" ] || echo "Test failed, code $ret" >&2
exit $ret

