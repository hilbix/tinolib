# $Header$
#
# $Log$
# Revision 1.4  2005-07-25 03:32:07  tino
# version with diet support
#
# Revision 1.3  2005/01/26 11:01:29  tino
# Changes for "make dist"
#
# Revision 1.2  2004/08/22 23:19:41  tino
# sync to cvs
#
# Revision 1.1  2004/02/14 20:26:44  tino
# Initial add

SUBS=old diet check

all it test:
	$(MAKE) -C old test

clean:
	for a in $(SUBS); do $(MAKE) -C $$a clean; done
	$(RM) *~

distclean:	clean
	$(RM) php/*~
	for a in $(SUBS); do $(MAKE) -C $$a distclean; done

dist:	distclean
	$(MAKE) -C old dist

diff:
	$(MAKE) -C old diff
