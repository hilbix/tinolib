#!/bin/sh
#
# $Header$
#
# Make a distribution .tar
# Everything is autodetecting
# Arguments:
#	cvscheck	Check CVS status
#	directory	will first cd there, defaults to .
#
# $Log$
# Revision 1.7  2005-04-24 12:55:38  tino
# started GAT support and filetool added
#
# Revision 1.6  2005/02/06 00:24:13  tino
# bugfix update
#
# Revision 1.5  2005/01/26 12:17:31  tino
# Enforced needed distribution files
#
# Revision 1.4  2004/07/21 13:29:14  tino
# Creation of standard Makefile from Makefile.tino added
#
# Revision 1.3  2004/07/05 01:56:18  tino
# forgot a not for make dist
#
# Revision 1.2  2004/07/03 11:03:28  tino
# corrections, and now a "make tar" does a .tmp.tgz
#
# Revision 1.1  2004/07/02 23:23:32  tino
# Moved tar generation to Makefile-tar.sh for new "make tar"

cd "${2:-.}"
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

for a in ANNOUNCE DESCRIPTION COPYING
do
	if [ ! -s "$here/$a" ]
	then
		echo "$a is missing or empty"
		exit 1
	fi
done

case "$1" in
tar)	VERS="$VERS.tmp";;
dist)	;;
*)	echo "internal error: neither 'tar' nor 'dist' command";;
esac

if [ -d "$here-$VERS" -o -f "$here-$VERS.tar.gz" ]
then
	echo "
exists: $here-$VERS
"
	exit 1
fi

tagcvs()
{
if !	(
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

# Perhaps in future GAT will do this for us on the fly
taggat()
{
if !	(
	cd "$here" &&
	gat cmp && gat publish
	)
then
	echo "
Please 'gat commit' before 'make dist'
"
	exit 1
fi
}

tagdist()
{
if [ -d "$here/GAT" ]
then
	taggat
elif [ -d "$here/CVS" ]
then
	tagcvs
else
	echo "Neither CVS nor GAT"
	exit 1
fi
}

case "$1" in
dist)	tagdist;;
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
