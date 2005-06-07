# $Header$
#
# As I want to compile something,
# and all the libraries out there seems to be broken in some respect or another,
# we have to compile everyting outself.
#
# Gather the neccessary sources, configure it and compile it.
#
# $Log$
# Revision 1.1  2005-06-07 20:41:57  tino
# This support is nearly not tested and not complete yet
#
# Revision 1.1  2005/06/01 01:53:32  tino
# initial add

TARGETS=libcurl.a libssl.a libcrypto.a diet libz.a
all:	$(TARGETS)

# What depends on what
libcurl.a::	libssl.a
libssl.a libcrypto.a::	libz.a
libz.a:: diet

# It takes too long to rebuild
# So clean does plain nothing
clean:
	$(RM) *~
	for a in $(TARGETS); do $(MAKE) -f "Makefile.$$a" clean; done
	@echo "To really clean tinodiet use one of the targets:"
	@echo "	makeclean	call 'make clean' in all subs"
	@echo "	confclean	remove all build subdirectories"
	@echo "	realclean	remove subs and cached .tar files"

distclean:
	for a in $(TARGETS); do $(MAKE) -f "Makefile.$$a" distclean; done

makeclean:
	for a in $(TARGETS); do $(MAKE) -f "Makefile.$$a" makeclean; done

confclean:
	for a in $(TARGETS); do $(MAKE) -f "Makefile.$$a" confclean; done

realclean:	distclean
	$(RM) -r build.distignore

diff:
	$(MAKE) -C .. diff

$(TARGETS)::
	$(MAKE) -f 'Makefile.$@' '$@'
