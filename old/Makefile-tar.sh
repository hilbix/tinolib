#!/bin/sh
#
# $Header$
#
# Make a distribution .tar
# Everything is autodetecting
# Arguments:
#	cvscheck	Check CVS status
#
# $Log$
# Revision 1.1  2004-07-02 23:23:32  tino
# Moved tar generation to Makefile-tar.sh for new "make tar"
#

for a in 1 2 3 4 5
do
	[ -f VERSION ] && break
	cd ..
done

here="`/bin/pwd`"
here="`basename "$here"`"
cd ..

if [ ! -r "$here/VERSION" ]
then
	echo "
$here/VERSION inaccessible
"
	exit 1
fi

VERS="`cat "$here/VERSION"`-`date +%Y%m%d-%H%M%S`"

if [ -d "$here-$VERS" -o -f "$here-$VERS.tar.gz" ]
then
	echo "
exists: $here-$VERS
"
	exit 1
fi

checkcvs()
{
if	(
	cd "$here" &&
	[ -z "`cvs diff 2>/dev/null | fgrep ========`" ] &&
	cvs tag -F "`echo "dist-$here-$VERS" | sed 's/[^-A-Za-z0-9]\\+/_/g'`"
	)
then
	echo "
Please 'cvs commit' before 'make dist'
"
	exit 1
fi
}

case "$1" in
tagcvs)	tagcvs;;
esac

mv -f "$here" "$here-$VERS" &&

tar -czf "$here-$VERS.tar.gz"		\
	--exclude "*.distignore"	\
	--exclude "*/CVS"		\
	--exclude "*/.cvsignore"	\
	--exclude "*.swp"		\
	"$here-$VERS" &&

mv -f "$here-$VERS" "$here" &&

echo "
distribution $here-$VERS created
"
