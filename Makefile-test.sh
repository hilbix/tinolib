#!/bin/bash
# $Header$
#
# Unit tests
#
# Copyright (C)2004-2006 Valentin Hilbig, webmaster@scylla-charybdis.com
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
# Revision 1.20  2008-05-19 09:09:58  tino
# iquote instead of -I-
#
# Revision 1.19  2007-09-17 17:45:09  tino
# Internal overhaul, many function names corrected.  Also see ChangeLog
#
# Revision 1.18  2007/08/06 16:00:55  tino
# Targets changed and make test / make info made better
#
# Revision 1.17  2007/04/11 16:09:11  tino
# See ChangeLog
#
# Revision 1.16  2007/01/25 04:39:15  tino
# Unit-Test now work for C++ files, too (and some fixes so that "make test" works).
#
# Revision 1.15  2006/10/04 00:00:32  tino
# Internal changes for Ubuntu 64 bit system: va_arg processing changed
#
# Revision 1.14  2006/08/23 01:19:17  tino
# See Changelog
#
# Revision 1.13  2006/07/31 23:15:01  tino
# -lexpat added for new xml.h
#
# Revision 1.12  2006/03/17 04:55:55  tino
# Minor improvements for make test
#
# Revision 1.11  2006/03/17 00:29:56  tino
# Improved method for "make test"
#
# Revision 1.10  2006/01/29 21:08:36  tino
# Though shalt not ci untested
#
# Revision 1.9  2006/01/29 17:49:52  tino
# Improved documentation and "make test"
#
# Revision 1.8  2005/12/05 02:11:12  tino
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
log+$2-$1:	UNIT$3_$1
	[ ! -f "UNIT$3_$1/LOG.out" ] || mv -f "UNIT$3_$1/LOG.out" "UNIT$3_$1/LOG.old"
	\$(MAKE) "$2-$1" 2>"UNIT$3_$1/LOG.out" || { \\
	err=\$\$?; \\
	hintline="\`grep '^\.\.\/\.\./' "UNIT$3_$1/LOG.out" | \\
	grep -v ':[0-9][0-9]*: warning: ' | \\
	sed -n '1,/:[0-9][0-9]*:/s/^......//p'\`"; \\
	[ -z "\$\$hintline" ] && hintline="\`head -10 "UNIT$3_$1/LOG.out"\`"; \\
	echo; \\
	echo "=====> $1: $2 failed"; echo "\$\$hintline"; \\
	exit \$\$err; }
	[ ! -s "UNIT$3_$1/LOG.old" -o -s "UNIT$3_$1/LOG.out" ] || mv -f "UNIT$3_$1/LOG.old" "UNIT$3_$1/LOG.out"

$2-$1:	UNIT$3_$1
	echo "+ $2 $1"
	\$(MAKE) -s -C "UNIT$3_$1" $2
EOF
}

echo
echo "Generating Makefile"

out-make <<EOF
# Automatically generated, do not edit!

CFLAGS=-iquote .. -DTINO_TEST_MAIN
CXXFLAGS=-iquote .. -DTINO_TEST_MAIN
LDLIBS=-lefence -lexpat -lcrypto

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

fail:	Makefile
	@echo
	@echo "failed targets:"
	@\$(MAKE) -sk fails
	@echo

bug:	Makefile
	@echo
	@echo "buggy targets:"
	@\$(MAKE) -sk buggy
	@echo

Makefile: ../Makefile-test.sh
	\$(MAKE) -s -C .. test

test:
	\$(MAKE) -s -C .. test

UNITc_%:	Makefile
	\$(RM) -r "\$@"; mkdir "\$@"
	ln -s ../Makefile.c.proto "\$@/Makefile"

UNITcc_%:	Makefile
	\$(RM) -r "\$@"; mkdir "\$@"
	ln -s ../Makefile.cc.proto "\$@/Makefile"

# for now only C include checking is supported
include: include-h include-hh
unit: unit-h unit-hh
manual: manual-h manual-hh
fails: fails-h fails-hh
buggy: buggy-h buggy-hh

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

	out-make "UNIT$ccext_$a:	../$a"

	gencc "$a" include "$ccext"
	if make -s -C "$BASE" "log+include-$a"
	then
		out-make "include-$incext:	log+include-$a"
		[ 0 = "$marker" ] && echo "$a: fails-marker still set" && FAILMARKER="$FAILMARKER $a"
	else
		if [ 0 = "$marker" ]
		then
			out-make "fails-$incext:	log+include-$a"
			FAILING="$FAILING $a"
			echo "	(that's ok, it's supposed to fail)"
		else
			out-make "buggy-$incext:	log+include-$a"
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
	out-make "unit-$incext:	log+unit-$a"
done

out-make <<EOF
info:
	@echo '-- Not working yet (which is OK): (make fail)'
	@echo '	$FAILING'
	@echo '-- Failmarker still set:'
	@echo '	$FAILMARKER'
	@echo '-- Buggy (no fail-marker but failing): (make bug)'
	@echo '	$BUGGY'

# Ready
EOF
touch -r "$BASE/Makefile.c.proto" "$BASE/Makefile.cc.proto" "$BASE/Makefile"
