#!/bin/sh
#
# Make a distribution .tar
# Everything is autodetecting
# Arguments:
#	cvscheck	Check CVS status
#	directory	will first cd there, defaults to .
#
# Copyright (C)2004-2014 Valentin Hilbig, webmaster@scylla-charybdis.com
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

ex()
{
echo "
ERROR: $*
" >&2
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

[ -r "$here/VERSION" ] || ex "$here/VERSION inaccessible"

VERS="`cat "$here/VERSION"`-`date +%Y%m%d-%H%M%S`"

checkall ANNOUNCE DESCRIPTION
checkany COPYING COPYRIGHT.CLL
checkedit ANNOUNCE DESCRIPTION VERSION

case "$1" in
tar)	VERS="$VERS.tmp";;
dist)	;;
*)	echo "internal error: neither 'tar' nor 'dist' command";;
esac

[ ! -d "$here-$VERS" -a ! -f "$here-$VERS.tar.gz" ] || ex "exists: $here-$VERS"

# Do the tagging

taggit()
{
(
LATEST="`echo "dist-$here" | sed 's/[^-A-Za-z0-9]\\+/_/g'`"
SPECIFIC="`echo "dist-$here-$VERS" | sed 's/[^-A-Za-z0-9]\\+/_/g'`"
cd "$here" || ex "cannot cd $here"
[ -z "`git status --porcelain`" ] || ex "Please 'git commit' before 'make dist'"
git tag -f "$LATEST" || ex "Cannot set tag $LASTEST"
git submodule foreach --recursive "git tag '$LATEST'" || ex "WTF? Cannot tag submodules?"
git tag "$SPECIFIC" || ex "Cannot set tag $LASTEST"
git submodule foreach --recursive "git tag '$SPECIFIC'" || "WTF?? Cannot tag submodules?"
) || exit 1
}

tagdist()
{
[ -d "$here/.git" ] || ex "no GIT repo dir, refusing to tag"
taggit
}

case "$1" in
dist)	tagdist;;
esac

# Do the TAR

mv -f "$here" "$here-$VERS" &&

tar -czf "$here-$VERS.tar.gz"		\
	--exclude "*.distignore"	\
	--exclude "*/.git"		\
	--exclude "*/.gitignore"	\
	--exclude "*.swp"		\
	"$here-$VERS" &&

mv -f "$here-$VERS" "$here" &&

echo "
distribution $here-$VERS created
"

