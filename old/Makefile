# $Header$
#
# $Log$
# Revision 1.5  2004-04-06 04:41:37  tino
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
	( while [ ! -f VERSION ]; do cd ..; done; \
	here="`/bin/pwd`"; here="`basename "$$here"`"; \
	VERS="`cat VERSION`-`date +%Y%m%d-%H%M%S`"; cd ..; \
	if [ -d "$$here-$$VERS" -o -f "$$here-$$VERS.tar.gz" ]; then \
		echo "exists: $$here-$$VERS"; \
	else \
		mv -f "$$here" "$$here-$$VERS"; \
		tar -czf "$$here-$$VERS.tar.gz" "$$here-$$VERS"; \
		mv -f "$$here-$$VERS" "$$here"; \
	fi; \
	)
