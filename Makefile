# $Header$
#
# $Log$
# Revision 1.2  2004-08-22 23:19:41  tino
# sync to cvs
#
# Revision 1.1  2004/02/14 20:26:44  tino
# Initial add
#

all:
	bin/gen tino

test:
	bin/gen test

diff:
	make -C old diff
