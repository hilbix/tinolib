# $Header$
#
# $Log$
# Revision 1.3  2005-01-26 11:01:29  tino
# Changes for "make dist"
#
# Revision 1.2  2004/08/22 23:19:41  tino
# sync to cvs
#
# Revision 1.1  2004/02/14 20:26:44  tino
# Initial add

all:

test:
	$(MAKE) -C old test

clean:
	$(RM) *~

distclean:	clean

dist:	distclean
	$(MAKE) -C old dist

diff:
	$(MAKE) -C old diff
