#!/bin/bash
# $Header$
#
# Unit tests
#
# Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Log$
# Revision 1.8  2005-12-05 02:11:12  tino
# Copyright and COPYLEFT added
#
# Revision 1.7  2005/06/04 14:35:06  tino
# Unit test improved
#
# Revision 1.6  2005/04/24 12:55:38  tino
# started GAT support and filetool added
#
# Revision 1.5  2005/01/26 10:46:41  tino
# CFLAGS corrected to use -I- to keep local include path from interfering with system header <includes>.
#
# Revision 1.4  2005/01/25 22:14:51  tino
# exception.h now passes include test (but is not usable).  See ChangeLog
#
# Revision 1.3  2005/01/04 13:23:49  tino
# see ChangeLog, mainly changes for "make test"
#
# Revision 1.2  2004/12/19 16:38:13  tino
# new unit test version for builtin unit tests, prepare to integrate in tinolib
#
# Revision 1.1  2004/09/04 20:15:22  tino
# started to add unit tests

set -e
cd "`dirname "$0"`"

case "$1" in
UNIT_TEST)	;;
*)		echo "ARG1 not UNIT_TEST" >&2; exit 1;;
esac

BASE="$1"
rm -rf "$BASE"
mkdir "$BASE"

genit()
{
cat <<EOF
#define TINO_FILE_EXCEPTION
#include "file.h"

$ALLINCLUDES

static int test_count=0;

#define TESTNOTNULL(X)	test_count++; if ((X)==0) { printf("%d TEST %s: fail !NULL %s\n", test_count, argv[1], #X); exit(0); }
#define TEST1(X)	test_count++; if ((X)!=1) { printf("%d TEST %s: fail TEST(1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST_1(X)	test_count++; if ((X)!=-1) { printf("%d TEST %s: fail TEST(-1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST0(X)	test_count++; if ((X)!=0) { printf("%d TEST %s: fail TEST(0) %s\n", test_count, argv[1], #X); exit(0); }
#define TESTCMP(X,Y)	test_count++; if (strcmp(X,Y)) { printf("%d TEST %s: fail CMP(%s,%s)=CMP('%s','%s')\n", test_count, argv[1], #X, #Y, X, Y); exit(0); }

int main(int argc, char **argv)
{
EOF

grep ^TEST "$1.h"

cat <<EOF
return 42;
}
EOF
}

runit()
{
set +e
( cd "$1" && exec ./"$2" "$2" ) >>"$1/LOG.out" 2>&1
ret="$?"
set -e
case "$ret" in
42)	echo "TEST $2: ok"; return 0;;
0)	echo "TEST $2: fail";;
*)	echo "TEST $2: returns $ret";;
esac
sed -n '$s/^/>>>/p' "$1/LOG.out"
false
}

hint()
{
grep ^\\. "$1" | head -1
}

testcc()
{
  TMP="$BASE/UNIT_TEST_$1"
  rm -rf "$TMP"
  mkdir "$TMP"
  cat >"$TMP/$1.c"
  cat >"$TMP/Makefile" <<EOF
CFLAGS=-Wall -g -I../.. -I-
LDLIBS=-lefence -lrt

all: $1
EOF
  if make -C "$TMP" >"$TMP/LOG.out" 2>&1
  then
	[ -z "$3" ] || "$3" "$TMP" "$1" || return 1
	rm -rf "$TMP"
  else
	echo "$1: $2: `hint "$TMP/LOG.out"`"
	return 1
  fi
}

ALLUNITTESTS=""
ALLINCLUDES=""

echo
echo "INCLUDE TESTS:"

cat >"$BASE/Makefile" <<EOF
CFLAGS=-I.. -I- -DTINO_TEST_MAIN
LDLIBS=-lefence
EOF
for a in *.h
do
	marker=0
	fgrep -x ' * UNIT TEST FAILS *' "$a" >/dev/null || marker=$?

	NAME="${a%.h}"
	if ! testcc "$NAME" "include failed" <<EOF
#include "$NAME.h"
int main(void) { return 0; }
EOF
	then
		[ 0 = "$marker" ] && echo "	(that's ok, it's supposed to fail)"
		continue
	fi
	[ 1 = "$marker" ] || echo "$NAME: fail marker still set"

	[ dirty != "$NAME" ] &&
	ALLINCLUDES="$ALLINCLUDES
#include \"$NAME.h\""

	if grep -q "^#ifdef[[:space:]]*TINO_TEST_MAIN" "$NAME.h"
	then
		ln -s "../$NAME.h" "$BASE/$NAME.c"
		cat >>"$BASE/Makefile" <<EOF
all:	$NAME
clean::
	\$(RM) $NAME
EOF
	fi

	grep -q "^#ifdef[[:space:]]*TINO_TEST_UNIT" "$NAME.h" &&
	ALLUNITTESTS="$ALLUNITTESTS $NAME"
done

# echo "$ALLINCLUDES"
echo
echo "UNIT TESTS:"
for a in $ALLUNITTESTS
do
	genit "$a" | testcc "$a" "compile failed" runit
done

echo
echo "MANUAL TESTS:"
make -C "$BASE"
