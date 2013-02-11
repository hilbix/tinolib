# This is the central Makefile of the "old" version of tinolib.
# (old means: tinolib does not yet have the structure I want it to have.)
#
# This Makefile mainly is a bunch of a lot of helpers.
# See README for information on how to use it.
#
# The usual targets:
# all	clean	distclean
# test (but see below)
#
# The magic targets (resembled in Makefile.proto too):
# tar	create an intermediate archive based on VERSION file.
#	This does not check GIT.  Use this instead of "dist"!
# tino	Create Makefile from Makefile.tino using Makefile.proto
#	This probably is no very clever name, however I cannot
#	call it "Makefile" as this is reserved for make internally.
#
# The magic targets which probably only help me:
# dist	create a new distribution archive based on VERSION file.
#	This makes sure everything is checked into CVS and
#	then does a CVS distribution tag.  Includes "distclean".
#	More magic may go into this in future, too.
# diff	Show what differs to CVS HEAD revision.
#	This greatly helps me to keep track of all those distrubuted
#	changes in my libs and programs etc.
#
# Test targets:
# check	MISSING TODAY (sadly)
#	Create a clean checked version (-lefence etc.)
#	This shall go before make test
# test	INCOMPLETE TODAY
#	Make check and run all unit tests.
#	This shall always go before make dist.

# Copyright (C)2004-2013 Valentin Hilbig, webmaster@scylla-charybdis.com
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

# Preset working directory, this can be overwritten from command line
HERE=$(PWD)

  BASH=bash
    CP=cp
    MV=mv -f
   CMP=cmp -s
  GAWK=gawk
   GIT=git
  ECHO=echo
  TEST=test
  DIFF=diff -u
 TOUCH=touch
 STRIP=strip
MDvSUM=md5sum
UNITTEST=UNIT_TEST

.PHONY: it all clean distclean dist tar diff tino fail test bug unit info

# Helping target for all too lazy people like me:
# Often I invoke 'M-x compile' in the tinolib directory
it:	all
	[ ".$(PWD)" = ".$(HERE)" ] && [ ! -f VERSION ] && \
	{ UP="`dirname "$(HERE)"`"; $(MAKE) -C"$$UP" it HERE="$$UP"; }

all:

lib.h:
	$(TOUCH) lib.h

clean:
	$(RM) *~
	$(RM) -r $(UNITTEST)

distclean:	clean
	$(RM) lib.h .#*

dist:	distclean
	if [ -n "$(DBG_CFLAGS)$(DBG_LDFLAGS)$(DBG_LDLIBS)$(DEBUGS)" ]; \
	then	echo ""; \
		echo "Debugging options still set in Makefile.tino"; \
		echo ""; \
		exit 1; \
	fi; \
	$(BASH) Makefile-tar.sh dist "$(HERE)"

tar:
	$(BASH) Makefile-tar.sh tar "$(HERE)"

diff:
	set -e; here="`pwd`"; cd "$(HERE)"; \
	for a in 1 2 3 4 5; do [ -f VERSION ] && break; cd ..; done; \
	$(GIT) status

tino:
	$(BASH) Makefile-tino.sh "$(HERE)" $(TINOCOPY)

check:
	false

fail:	$(UNITTEST)/Makefile
	$(MAKE) -s -C $(UNITTEST) fail

test:	$(UNITTEST)/Makefile
	$(MAKE) -s -C $(UNITTEST)

bug:	$(UNITTEST)/Makefile
	$(MAKE) -s -C $(UNITTEST) bug

unit:	$(UNITTEST)/Makefile
	$(MAKE) -s -C $(UNITTEST) unit

info:	$(UNITTEST)/Makefile
	$(MAKE) -s -C $(UNITTEST) info

$(UNITTEST)/Makefile:	Makefile Makefile-test.sh
	$(RM) -r $(UNITTEST)
	$(BASH) Makefile-test.sh $(UNITTEST)
