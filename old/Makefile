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
# Revision 1.15  2004-07-28 03:44:26  tino
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

    SH=sh
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

distclean:	clean
	$(RM) lib.h

dist:	distclean
	$(SH) Makefile-tar.sh dist "$(HERE)"

tar:
	$(SH) Makefile-tar.sh tar "$(HERE)"

diff:
	set -e; here="$$PWD"; cd "$(HERE)"; \
	for a in 1 2 3 4 5; do [ -f VERSION ] && break; cd ..; done; \
	$(CVS) status -v 2>&1 | $(AWK) -f "$$here/Makefile-diff.awk"

tino:
	@set -e; here="$$PWD"; cd "$(HERE)"; $(TEST) -f Makefile.tino; \
	$(AWK) -vSRC="$$here" -f"$$here/Makefile.awk" Makefile.tino "$$here/Makefile.proto" >Makefile.~; \
	$(MDvSUM) < Makefile.~ > Makefile.~~; \
	if $(CMP) Makefile Makefile.~; then \
		$(RM) Makefile.~; \
		$(ECHO) "Makefile is up to date."; \
	elif [ ! -f Makefile ] || $(MDvSUM) <Makefile | $(CMP) Makefile.md5 -; \
	then \
		$(ECHO) "Updated Makefile."; \
		$(MV) Makefile.~ Makefile; \
	else \
		$(DIFF) Makefile Makefile.~ | sed 's/^/|/'; \
		$(ECHO) "Integrity test of existing Makefile failed."; \
		$(ECHO) "New Makefile was written to Makefile.~"; \
		$(ECHO) "To install the new version please do:"; \
		$(ECHO) "	mv -i Makefile.~ Makefile"; \
	fi; \
	$(TOUCH) -r Makefile -B 1 Makefile.~~; \
	$(MV) Makefile.~~ Makefile.md5
# The latter is no bug!  This way the printed mv works!

check:
	false

test:
	false
