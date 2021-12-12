#

HEADS=$(wildcard *.h)
TESTS=$(HEADS:.h=)

.PHONY:	love
love:	all

.PHONY:	all
all:	test

.PHONY:	test
test:
	for a in $(TESTS); do cc -D__A_TEST__ -xc "$$a.h" -o "$$a" && "./$$a" || exit; done

