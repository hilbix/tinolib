#!/bin/sh
#
# This simple wrapper to diet is public domain.
#
# Arguments:
# ++		Use g++
# --tinodiet	This is a real call, not a compatibility call
#
# This fixes various things by prefixing the diet call:
#
# 1) zlib is braindeadly checking the output.
#    If there is any diet warning this is treated as an error.
#    So supporess any warnings as long as diet returns 0
#
# 2) libcurl uses u_char but forgets to set -D_BSD_SOURCE

rundiet="`which diet`"
[ -z "$rundiet" ] && rundiet="`dirname "$0"`/diet"

TINODIETCC=gcc
if [ .++ = ".$1" ]
then
	shift
	TINODIETCC=g++
fi

# Only trust the caller knows about diet when "--tinodiet" is set
if [ .--tinodiet = ".$1" ]
then
	shift
	"$rundiet" -v -Os $TINODIETCC -DTINO_DIET_COMPILE "$@"
	exit
fi

# Else the caller might have some trouble with diet
# Add some standard
TMP=/tmp/mydiet.$$.out
"$rundiet" -v $TINODIETCC -D_BSD_SOURCE "$@" >"$TMP" 2>&1
ret=$?
[ 0 != $ret ] && cat "$TMP"
rm -f "$TMP"
exit $ret
