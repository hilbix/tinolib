#!/bin/sh
# $Header$
#
# Unit tests
#
# $Log$
# Revision 1.2  2004-12-19 16:38:13  tino
# new unit test version for builtin unit tests, prepare to integrate in tinolib
#
# Revision 1.1  2004/09/04 20:15:22  tino
# started to add unit tests
#

genit()
{
cat <<EOF
#include "tino/file.h"

$ALLINCLUDES

static int test_count=0;

#define TESTNOTNULL(X)	test_count++; if ((X)==0) { printf("%d TEST %s: fail !NULL %s\n", test_count, argv[1], #X); exit(0); }
#define TEST1(X)	test_count++; if ((X)!=1) { printf("%d TEST %s: fail TEST(1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST_1(X)	test_count++; if ((X)!=-1) { printf("%d TEST %s: fail TEST(-1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST0(X)	test_count++; if ((X)!=0) { printf("%d TEST %s: fail TEST(0) %s\n", test_count, argv[1], #X); exit(0); }

int main(int argc, char **argv)
{
EOF

grep ^TEST "tino/$1.h"

cat <<EOF
return 42;
}
EOF
}

makeit()
{
  cat >"$TMP/Makefile" <<EOF
CFLAGS=-Wall -O3

all: run
EOF
  make -C "$1" >"$1/LOG.out" 2>&1
}

ALLUNITTESTS=""
ALLINCLUDES=""

echo "INCLUDE TESTS:"
rm -rf GEN
mkdir GEN
ln -s ../tino GEN
echo "CFLAGS=-Itino -DTINO_TEST_MAIN" > GEN/Makefile
for a in tino/*.h
do
	NAME="`basename "$a" .h`"
	TMP="TEST_$NAME"
	rm -rf "$TMP"
	mkdir "$TMP" || exit
	ln -s ../tino "$TMP" || exit
	cat >"$TMP/run.c" <<EOF
#include "$a"
int main(void) { return 0; }
EOF

	if ! makeit "$TMP"
	then
		echo "TEST $NAME: include failed"
		continue
	fi
	rm -rf "$TMP"

	[ dirty != "$NAME" ] &&
	ALLINCLUDES="$ALLINCLUDES
#include \"$a\""

	if grep -q "^#ifdef[[:space:]]*TINO_TEST_MAIN" "$a"
	then
		ln -s "tino/$NAME.h" "GEN/$NAME.c"
		cat >>GEN/Makefile <<EOF
all:	$NAME
$NAME:	$NAME.c
$NAME.c:	$a
clean::
	\$(RM) $NAME
EOF
	fi

	grep -q "^#ifdef[[:space:]]*TINO_TEST_UNIT" "$a" &&
	ALLUNITTESTS="$ALLUNITTESTS $NAME"
done

echo
echo "GENERIC TESTS:"
make -C GEN

# echo "$ALLINCLUDES"

echo
echo "UNIT TESTS:"
ok=:
for a in $ALLUNITTESTS
do
	TMP="TEST_$a"
	rm -rf "$TMP"
	mkdir "$TMP" || exit
	ln -s ../tino "$TMP" || exit
	genit "$a" >"$TMP/run.c"
	if ! makeit "$TMP"
	then
		echo "TEST $a: compile failed"
		continue
	fi

	( cd "$TMP" && exec ./run "$a" ) >>"$TMP/LOG.out" 2>&1
	ret="$?"
	case "$ret" in
	42)	echo "TEST $a: ok"; rm -rf "$TMP";;
	0)	echo "TEST $a: fail";;
	*)	echo "TEST $a: returns $ret";;
	esac
done
$ok
