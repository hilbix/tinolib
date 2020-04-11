#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

PROG=$(patsubst %.c,%,$(wildcard *.c))

GITVER=$(shell git rev-parse --short HEAD)
GITURL=$(patsubst .git,,$(shell git config --local --get remote.origin.url))
CFLAGS=-Wall -O3 -DGIT_VER='g$(GITVER)' -DGIT_URL='$(GITURL)'

INCS=$(patsubst lib/%,%,$(wildcard lib/*.h))
TESTS=$(patsubst %.h,tests/%.c,$(wildcard *.h))

.PHONY:	love
love:	all
	[ ! -f ../Makefile ] || $(MAKE) -C.. '$@'

.PHONY:	all
all:	$(INCS) tests $(TESTS)

$(INCS):	Makefile bin/gen.sh

tests:	test
	mkdir '$@'

test:	Makefile
	mkdir -p '$@'
	cd '$@' && for a in ../user/* ../user/.*[^.]* .; do [ -e "$$a" ] && ln -s "$$a" .; :; done
	cd '$@' && ln -s .. minilib || :
	{ echo '#define TEST_MAIN' && cat all.h; } > '$@/minilibtest.c'

%.h:	lib/%.h
	bin/gen.sh '$@' 'lib/$@'

tests/%.c:	%.h
	{ echo '#define TEST_MAIN' && cat '$<'; } > '$@'

.PHONY:	clean
clean:
	bin/clean.sh $(INCS)
	$(RM) .generated.checksum $(PROG)
	$(RM) -r test/ tests/

