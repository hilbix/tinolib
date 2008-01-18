#!/bin/sh
# $Header$
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
# 
# $Log$
# Revision 1.3  2008-01-18 03:17:35  tino
# diet now uses -Os
#
# Revision 1.2  2007-10-04 12:57:00  tino
# See ChangeLog
#
# Revision 1.1  2006-12-02 10:28:44  tino
# Moved from ../diet to here
#
# Revision 1.2  2005/08/02 03:08:57  tino
# Added C++ option:  tinodiet.sh ++ --tinodiet
#
# Revision 1.1  2005/06/07 20:41:57  tino
# This support is nearly not tested and not complete yet

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
	"$rundiet" -Os $TINODIETCC -DTINO_DIET_COMPILE "$@"
	exit
fi

# Else the caller might have some trouble with diet
# Add some standard
TMP=/tmp/mydiet.$$.out
"$rundiet" $TINODIETCC -D_BSD_SOURCE "$@" >"$TMP" 2>&1
ret=$?
[ 0 != $ret ] && cat "$TMP"
rm -f "$TMP"
exit $ret
