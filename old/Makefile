# $Header$
#
# This is the central Makefile of the "old" version of tinolib.
# (old means: tinolib does not yet have the structure I want it to have.)
#
# This Makefile mainly is a bunch of a lot of helpers.
# See Makefile.proto for information on how to use it.
#
# The usual targets:
# all	clean	distclean
#
# The magic targets (resembled in Makefile.proto too):
# tar	create an intermediate archive based on VERSION file.
#	This does not check CVS.  Use this instead of "dist"!
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
# The missing targets, sadly:
# check	MISSING TODAY
#	Create a clean checked version (-lefence etc.)
# test	MISSING TODAY
#	Make check and run all unit tests.
#	This shall always go before make dist.
#
# $Log$
# Revision 1.19  2005-01-04 13:23:49  tino
# see ChangeLog, mainly changes for "make test"
#
# Revision 1.18  2004/09/29 00:00:47  tino
# make install and make dist, see Changelog
#
# Revision 1.17  2004/08/22 05:47:34  Administrator
# Now Makefile generator understands to look into variables set in Makefile.
# This way Auto-Dependicies can be improved a little step and more important,
# the "TINOCOPY" functionality was added, such that I can copy files from
# elsewhere to the distribution and keep that files in sync.
#
# Revision 1.16  2004/08/15 21:37:55  Administrator
# Cygwin changes
#
# Revision 1.15  2004/07/28 03:44:26  tino
# Makefile changes
#
# Revision 1.14  2004/07/21 13:29:14  tino
# Creation of standard Makefile from Makefile.tino added
#
# Revision 1.13  2004/07/03 11:03:28  tino
# corrections, and now a "make tar" does a .tmp.tgz
#
# Revision 1.12  2004/07/02 23:23:32  tino
# Moved tar generation to Makefile-tar.sh for new "make tar"
#
# Revision 1.11  2004/06/12 08:34:25  tino
# "make diff" added
#
# Revision 1.9  2004/06/06 05:06:02  tino
# first version of CVS autoTAG on make dist
#
# Revision 1.6  2004/05/09 20:56:30  tino
# make dist changes: *.distignore are ignored and make distclean on dist

# Preset working directory, this can be overwritten from command line
HERE=$(PWD)

  BASH=bash
    CP=cp
    MV=mv -f
   CMP=cmp -s
   AWK=awk
   CVS=cvs
  ECHO=echo
  TEST=test
  DIFF=diff -u
 TOUCH=touch
 STRIP=strip
MDvSUM=md5sum
UNITTEST=UNIT_TEST

.PHONY: it all clean distclean dist tar diff tino

# Helping target for all too lazy people like me:
# Often I invoke 'M-x compile' in the tinolib directory
it:	all
	[ ".$(PWD)" = ".$(HERE)" ] && [ ! -f VERSION ] && \
	{ UP="`dirname "$(HERE)"`"; $(MAKE) -C"$$UP" it HERE="$$UP"; }

all:
	[ -f lib.h ] || $(TOUCH) lib.h
	[ -z "`find . -type f -name '*.h' -newer lib.h`" ] || $(TOUCH) lib.h

lib.h:	all

clean:
	$(RM) *~
	$(RM) -r $(UNITTEST)

distclean:	clean
	$(RM) lib.h

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
	$(CVS) status -v 2>&1 | $(AWK) -f "$$here/Makefile-diff.awk"

tino:
	$(BASH) Makefile-tino.sh "$(HERE)" $(TINOCOPY)

check:
	false

test:
	$(BASH) Makefile-test.sh $(UNITTEST)
