#!/bin/bash
# $Header$
#
# Unit tests
#
# $Log$
# Revision 1.5  2005-01-26 10:46:41  tino
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
( cd "$1" && exec ./"$2" ) >>"$1/LOG.out" 2>&1
ret="$?"
set -e
case "$ret" in
42)	echo "TEST $2: ok"; return 0;;
0)	echo "TEST $2: fail";;
*)	echo "TEST $2: returns $ret";;
esac
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
CFLAGS=-Wall -O3 -I../.. -I-
LDLIBS=-lefence

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
	NAME="${a%.h}"
	testcc "$NAME" "include failed" <<EOF || continue
#include "$NAME.h"
int main(void) { return 0; }
EOF

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
