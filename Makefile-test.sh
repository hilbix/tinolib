#!/bin/bash
#
# Unit tests
#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

set -e
cd "`dirname "$0"`"

case "$1" in
UNIT_TEST)	;;
*)		echo "ARG1 not UNIT_TEST" >&2; exit 1;;
esac

BASE="$1"
[ ! -d "$BASE" ] &&
mkdir "$BASE"

> "$BASE/Makefile"

cat >"$BASE/test-all.h" <<EOF
#define TINO_FILE_EXCEPTION
#include "alarm.h"
EOF

cat >"$BASE/test-all.hh" <<EOF
EOF

out-inc()
{
tee -a "$BASE/test-all.h" >>"$BASE/test-all.hh"
}

add-inc()
{
for add
do
	case "$add" in
	*.h)	echo "#include \"$add\"" >>"$BASE/test-all.h";;
	*.hh)	echo "#include \"$add\"" >>"$BASE/test-all.hh";;
	esac
done
}

out-inc <<EOF
static int test_count=0;

#define TESTNOTNULL(X)	test_count++; if ((X)==0) { printf("%d TEST %s: fail !NULL %s\n", test_count, argv[1], #X); exit(0); }
#define TEST1(X)	test_count++; if ((X)!=1) { printf("%d TEST %s: fail TEST(1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST_1(X)	test_count++; if ((X)!=-1) { printf("%d TEST %s: fail TEST(-1) %s\n", test_count, argv[1], #X); exit(0); }
#define TEST0(X)	test_count++; if ((X)!=0) { printf("%d TEST %s: fail TEST(0) %s\n", test_count, argv[1], #X); exit(0); }
#define TESTCMP(X,Y)	test_count++; if (strcmp(X,Y)) { printf("%d TEST %s: fail CMP(%s,%s)=CMP('%s','%s')\n", test_count, argv[1], #X, #Y, X, Y); exit(0); }
EOF

for a in c cc
do
	cat > "$BASE/Makefile.$a.proto" <<EOF
# Automatically generated, do not edit!

CFLAGS=-Wall -Wno-unused-function -g -iquote .. -iquote ../..
CXXFLAGS=-Wall -Wno-unused-function -g -iquote .. -iquote ../..
LDLIBS=-lefence -lrt -lexpat -lcrypto

all:
	\$(MAKE) -s -C .. "\`basename "\$(PWD)"\`"

test:
	\$(MAKE) -s -C .. \$@

include.$a:	Makefile
	a="\`basename "\$\$PWD" | sed 's/^UNIT[^_]*_//'\`"; ( \\
	echo "/* Automatically generated, do not edit */"; \\
	echo "#include \\"\$\$a\\""; \\
	echo "int main(void) { return 0; }"; \\
	) > "include.$a"

compile.$a:	Makefile
	a="\`basename "\$\$PWD" | sed 's/^UNIT[^_]*_//'\`"; ( \\
	echo '#include "test-all.h"'; \\
	echo 'int main(int argc, char **argv) {'; \\
	grep ^TEST "../../\$\$a"; \\
	echo 'return 42; }'; \\
	) > "compile.$a"

unit:	compile
	a="\`basename "\$\$PWD" | sed 's/^UNIT[^_]*_//'\`"; \\
	./compile "\$\$a" 2>UNIT.log >&2; \\
	ret=\$\$?; \\
	case \$\$ret in \\
	42) exit 0;; \\
	0)  echo "FAIL \$\$a: test condition violated";; \\
	*)  echo "FAIL \$\$a: returns \$ret";; \\
	esac; \\
	sed -n '\$\$s/^/>>>/p' UNIT.log
EOF
done

out-make()
{
if [ 0 != $# ]
then
	echo "$*"
else
	cat
fi >>"$BASE/Makefile"
}

gencc()
{
  out-make <<EOF
$1:	$2-$1
.PHONY:	logged+$2-$1
logged+$2-$1:	UNIT$3_$1
	[ ! -f "UNIT$3_$1/LOG-$2.out" ] || mv -f "UNIT$3_$1/LOG-$2.out" "UNIT$3_$1/LOG-$2.old"
	\$(MAKE) "$2-$1" 2>"UNIT$3_$1/LOG-$2.out" || { \\
	err=\$\$?; \\
	hintline="\`grep '^\.\.\/\.\./' "UNIT$3_$1/LOG-$2.out" | \\
	grep -v ':[0-9][0-9]*: warning: ' | \\
	sed -n '1,/:[0-9][0-9]*:/s/^......//p'\`"; \\
	[ -z "\$\$hintline" ] && hintline="\`head -10 "UNIT$3_$1/LOG-$2.out"\`"; \\
	echo; \\
	echo "=====> $1: $2 failed, see: make log+$2-$1"; echo "\$\$hintline"; \\
	exit \$\$err; }
	[ ! -s "UNIT$3_$1/LOG-$2.out" ] || rm -f "UNIT$3_$1/LOG-$2.old" 

UNIT$3_$1/LOG-$2.out:
	\$(MAKE) logged+$2-$1

.PHONY:	log+$2-$1
log+$2-$1:	UNIT$3_$1/LOG-$2.out
	-@[ -s '\$<' ] && echo && echo '======== \$< ========' && echo && cat -n '\$<'

.PHONY:	$2-$1
$2-$1:	UNIT$3_$1
	echo "+ $2 $1"
	\$(MAKE) -s -C "UNIT$3_$1" $2
	! fgrep -w DP '../$1' || { echo "### Compiles ok but still uses DP macro ###" >&2; false; }
EOF
}

echo
echo "Generating Makefile"

out-make <<EOF
# Automatically generated, do not edit!

CFLAGS=-iquote .. -DTINO_TEST_MAIN -Wno-error=unused-function -Wno-error=unused-value
CXXFLAGS=-iquote .. -DTINO_TEST_MAIN -Wno-error=unused-function -Wno-error=unused-value
LDLIBS=-lefence -lexpat -lcrypto

.PHONY:	all
all:	Makefile
	@echo
	@echo "INCLUDE TESTS: (make include)"
	-@\$(MAKE) -sk include
	@echo
	@echo "UNIT TESTS: (make unit)"
	-@\$(MAKE) -sk unit
	@echo
	@echo "MANUAL TESTS: (make manual)"
	-@\$(MAKE) -sk manual
	@echo
	@echo "INFO: (make info)"
	-@\$(MAKE) -sk info
	@echo

.PHONY:	fail
fail:	Makefile
	@echo
	@echo "failed targets:"
	@\$(MAKE) -sk fails
	@echo

.PHONY:	bug
bug:	Makefile
	@echo
	@echo "buggy targets:"
	@\$(MAKE) -sk buggy
	@echo

Makefile: ../Makefile-test.sh
	\$(MAKE) -s -C .. test

.PHONY:	test
test:
	\$(MAKE) -s -C .. test

UNITc_%:	Makefile
	\$(RM) -r "\$@"; mkdir "\$@"
	ln -s ../Makefile.c.proto "\$@/Makefile"

UNITcc_%:	Makefile
	\$(RM) -r "\$@"; mkdir "\$@"
	ln -s ../Makefile.cc.proto "\$@/Makefile"

# for now only C include checking is supported
.PHONY:	include unit manual fails buggy
include: include-h include-hh
unit: unit-h unit-hh
manual: manual-h manual-hh
fails: fails-h fails-hh
buggy: buggy-h buggy-hh

.PHONY:	buggy-h buggy-hh fails-h fails-hh include-h include-hh unit-h unit-hh manual-h manual-hh
buggy-h:
buggy-hh:
fails-h:
fails-hh:
include-h:
include-hh:
unit-h:
unit-hh:
manual-h:
manual-hh:

.PHONY:	log+all log+fail log+info log+test log+unit log+bug
log+all:	log+fail log+info log+test log+unit log+bug
log+fail:
log+info:
log+test:
log+bug:
log+unit:

.PHONY:	clean
EOF

for a in *.h *.hh
do
	case "$a" in
	*.h)	ccext="c"; incext="h";;
	*.hh)	ccext="cc"; incext="hh";;
	esac
	NAME="${a%.$incext}"

	marker=0
	fgrep -x ' * UNIT TEST FAILS *' "$a" >/dev/null || marker=$?

	out-make "UNIT${ccext}_$a:	../$a"

	gencc "$a" include "$ccext"
	if make -s -C "$BASE" "logged+include-$a"
	then
		out-make "include-$incext:	logged+include-$a"
		if	[ 0 = "$marker" ]
		then
			echo "$a: fails-marker still set" && FAILMARKER="$FAILMARKER $a"
			out-make "log+info:	log+include-$a"
		else
			out-make "log+test:	log+include-$a"
		fi

	else
		if [ 0 = "$marker" ]
		then
			out-make ".PHONY:	fails-$incext"
			out-make "fails-$incext:	logged+include-$a"
			out-make "log+fail:	log+include-$a"
			FAILING="$FAILING $a"
			echo "	(that's ok, it's supposed to fail)"
		else
			out-make ".PHONY:	buggy-$incext"
			out-make "buggy-$incext:	logged+include-$a"
			out-make "log+bug:	log+include-$a"
			BUGGY="$BUGGY $a"
		fi
		echo
		continue
	fi

	[ dirty != "$NAME" ] &&
	add-inc "$a"

	grep -q "^#ifdef[[:space:]]*TINO_TEST_MAIN" "$a" &&
	out-make <<EOF
$a: $NAME
.PHONY:	manual-$NAME
manual-$NAME:	$NAME
	echo "+ manual $NAME"
manual-$incext:	manual-$NAME
$NAME.$ccext:
	ln -s "../$a" "\$@"
clean::
	\$(RM) "$NAME" "$NAME.$ccext"
EOF

	grep -q "^#ifdef[[:space:]]*TINO_TEST_UNIT" "$a" &&
	gencc "$a" unit "$ccext" &&
	out-make "log-unit:	log+unit-$a" &&
	out-make "unit-$incext:	logged+unit-$a"
done

out-make <<EOF
.PHONY:	info
info:
	@echo '-- Not working yet (which is OK): make fail log+fail'
	@echo '	$FAILING'
	@echo '-- Failmarker still set:  make log+info'
	@echo '	$FAILMARKER'
	@echo '-- Buggy (no fail-marker but failing): make bug log+bug'
	@echo '	$BUGGY'
	@echo '-- other log targets: make log+unit log+test'

# Ready
EOF
touch -r "$BASE/Makefile.c.proto" "$BASE/Makefile.cc.proto" "$BASE/Makefile"

