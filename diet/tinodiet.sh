#!/bin/sh
# $Header$
#
# This is just a wrapper to diet, which is public domain.
#
# This fixes various things by prefixing the diet call:
#
# 1) zlib is braindeadly checking the output.
#    If there is any diet warning this is treated as an error.
#    So supporess any warnings as long as diet returns 0
#
# 2) libcurl uses u_char but forgets to set -D_BSD_SOURCE
# 
# $Log$
# Revision 1.1  2005-06-07 20:41:57  tino
# This support is nearly not tested and not complete yet
#

# Only trust the caller knows about diet when "--tinodiet" is set
if [ .--tinodiet = ".$1" ]
then
	shift
	"`dirname "$0"`/diet" gcc -DTINO_DIET_COMPILE "$@"
	exit
fi

# Else the caller might have some trouble with diet
# Add some standard
TMP=/tmp/mydiet.$$.out
"`dirname "$0"`/diet" gcc -D_BSD_SOURCE "$@" >"$TMP" 2>&1
ret=$?
[ 0 != $ret ] && cat "$TMP"
rm -f "$TMP"
exit $ret
