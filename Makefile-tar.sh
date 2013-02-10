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
# Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Log$
# Revision 1.13  2008-09-27 16:55:05  tino
# GAT fixes
#
# Revision 1.12  2007-09-26 13:27:03  tino
# COPYRIGHT.CLL now is supported, too.
#
# Revision 1.11  2006/08/01 00:31:09  tino
# Make dist worked when it should not work
#
# Revision 1.10  2006/06/11 19:47:25  tino
# See ChangeLog
#
# Revision 1.9  2005/12/05 02:11:12  tino
# Copyright and COPYLEFT added
#
# Revision 1.8  2005/07/30 16:13:24  tino
# Changes to enable setuptino.sh to newly setup an empty directory
#
# Revision 1.7  2005/04/24 12:55:38  tino
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

ex()
{
echo "$*" >&2
exit 1
}

cd "${2:-.}"
for a in 1 2 3 4 5
do
	[ -f VERSION ] && break
	cd .. || ex "cannot cd .."
done

here="`/bin/pwd`"
here="`basename "$here"`"
cd .. || ex "cannot cd .."

checkall()
{
for a
do
	[ -s "$here/$a" ] ||
	ex "$a is missing or empty"
done
}

checkany()
{
for a
do
	[ -s "$here/$a" ] && return
done
ex "all files of following list are missing: $*"
}

checkedit()
{
for a
do
	[ -f "tino/$a" -a -f "$a" ] &&
	cmp -s "tino/$a" "$a" &&
	ex "$a is still not edited!"
done
}

# Check prerequisites

[ -r "$here/VERSION" ] ||
ex "
$here/VERSION inaccessible
"

VERS="`cat "$here/VERSION"`-`date +%Y%m%d-%H%M%S`"

checkall ANNOUNCE DESCRIPTION
checkany COPYING COPYRIGHT.CLL
checkedit ANNOUNCE DESCRIPTION VERSION

case "$1" in
tar)	VERS="$VERS.tmp";;
dist)	;;
*)	echo "internal error: neither 'tar' nor 'dist' command";;
esac

[ ! -d "$here-$VERS" -a ! -f "$here-$VERS.tar.gz" ] ||
ex "
exists: $here-$VERS
"

# Do the tagging

tagcvs()
{
(
cd "$here" &&
[ -z "`cvs diff 2>/dev/null | fgrep ========`" ] &&
cvs tag -F "`echo "dist-$here" | sed 's/[^-A-Za-z0-9]\\+/_/g'`" &&
cvs tag -F "`echo "dist-$here-$VERS" | sed 's/[^-A-Za-z0-9]\\+/_/g'`"
) ||
ex "
Please 'cvs commit' before 'make dist'
"
}

# Perhaps in future GAT will do this for us on the fly
taggat()
{
(
cd "$here" &&
gat cmp && gat publish
) ||
ex "
Please 'gat commit' before 'make dist'
"
}

tagdist()
{
if [ -d "$here/+GAT" ]
then
	taggat
elif [ -d "$here/CVS" ]
then
	tagcvs
else
	ex "Neither CVS nor GAT"
fi
}

case "$1" in
dist)	tagdist;;
esac

# Do the TAR

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
