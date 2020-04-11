#

PROG=$(patsubst %.c,%,$(wildcard *.c))

GITVER=$(shell git rev-parse --short HEAD)
GITURL=$(patsubst .git,,$(shell git config --local --get remote.origin.url))
CFLAGS=-Wall -O3 -DGIT_VER='g$(GITVER)' -DGIT_URL='$(GITURL)'

INCS=$(patsubst lib/%,%,$(wildcard lib/*.h))
TESTS=$(patsubst %.h,tests/%.c,$(wildcard *.h))

.PHONY:	love
love:	all
	echo $(INCS)
	[ -f ../Makefile ] && $(MAKE) -C.. '$@'

.PHONY:	all
all:	$(INCS) tests $(TESTS)

tests:	test
	mkdir '$@'

test:
	mkdir '$@'
	ln -s ../Makefile.test '$@/Makefile'
	ln -s .. '$@/minilib'
	{ echo '#define TEST_MAIN' && cat all.h; } > '$@/minilibtest.c'

%.h:	lib/%.h
	bin/gen.sh '$@' '$<'

tests/%.c:	%.h
	{ echo '#define TEST_MAIN' && cat '$<'; } > '$@'

.PHONY:	clean
clean:
	$(RM) $(PROG)
	$(RM) -r test/ tests/

