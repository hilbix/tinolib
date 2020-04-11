#

WFLAGS=-Wno-unused-function
CFLAGS=-Wall -O3 -DGIT_VER='"g$(GITVER)"' -DGIT_URL='"$(GITURL)"' $(WFLAGS)

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)
PROG=$(SRCS:.c=)

GITVER=$(shell git rev-parse --short HEAD)
GITURL=$(patsubst .git,,$(shell git config --local --get remote.origin.url))

.PHONY:	love
love:	all

.PHONY:	all
all:	minilib $(PROG)

.PHONY:	clean
clean:
	$(RM) $(PROG) $(OBJS) $(DEPS)
	$(MAKE) -C minilib "$@"

.PHONY:	minilib
minilib:
	[ -e minilib/.git ] || git submodule update --init
	$(MAKE) -C minilib all

-include $(DEPS)

# minilib is needed before the depends targets
%.d:    %.c	| minilib
	@echo depends $@
	@rm -f '$@' && $(CC) -MM $(CFLAGS) '$<' | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' >'$@'

