# $Header$
#
# $Log$
# Revision 1.12  2004-07-02 23:23:32  tino
# Moved tar generation to Makefile-tar.sh for new "make tar"
#
# Revision 1.11  2004/06/12 08:34:25  tino
# "make diff" added
#
# Revision 1.10  2004/06/06 05:26:58  tino
# second version, now checks if cvs commit was forgotten
#
# Revision 1.9  2004/06/06 05:06:02  tino
# first version of CVS autoTAG on make dist
#
# Revision 1.8  2004/05/21 02:11:23  tino
# .cvsignore added to excludes
#
# Revision 1.7  2004/05/20 04:26:28  tino
# make dist now ignores CVS and .swp files, too
#
# Revision 1.6  2004/05/09 20:56:30  tino
# make dist changes: *.distignore are ignored and make distclean on dist
#
# Revision 1.5  2004/04/06 04:41:37  tino
# xml reader
#
# Revision 1.4  2004/04/05 00:44:35  tino
# still dist changes
# now "make" works
#
# Revision 1.3  2004/04/04 22:46:54  tino
# dist
#
# Revision 1.2  2004/03/28 00:08:21  tino
# Some more added, bic2sql works now
#
# Revision 1.1  2004/03/26 20:17:50  tino
# More little changes

.PHONY: all
all:
	[ -f lib.h ] || touch lib.h
	[ -z "`find . -type f -name '*.h' -newer lib.h`" ] || touch lib.h

lib.h:	all

clean:
	$(RM) *~

distclean:	clean
	$(RM) lib.h

dist:	distclean
	sh Makefile-tar.sh cvscheck

tar:
	sh Makefile-tar.sh nocvscheck

diff:
	( here="$$PWD"; \
	for a in 1 2 3 4 5; do [ -f VERSION ] && break; cd ..; done; \
	cvs status -v 2>&1 | awk -f "$$here/Makefile-diff.awk"; \
	)
