#!/bin/bash
# $Header$
#
# Maintain Makefile.md5 file, a list of MD5 sums of files which are
# automatically generated (like the Makefile).
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
# Revision 1.9  2006-10-03 20:26:52  tino
# Ubuntu has no gawk as awk, so gawk used instead of awk
#
# Revision 1.8  2006/03/19 20:49:46  tino
# corrected line include offset
#
# Revision 1.7  2006/02/12 01:38:48  tino
# TINOCOPY now adds line number for more easy editing (only for me, not for you).
#
# Revision 1.6  2005/12/05 02:11:12  tino
# Copyright and COPYLEFT added
#
# Revision 1.5  2005/08/01 17:53:28  tino
# C++ additions
#
# Revision 1.4  2004/09/04 14:25:20  tino
# typos corrected
#
# Revision 1.3  2004/09/04 14:12:15  tino
# Automated dependencies added and other make improvements.
#
# Revision 1.2  2004/08/24 23:49:36  tino
# Feature MD5TINOIGN to ignore ever changing output lines from MD5 checks.
#
# Revision 1.1  2004/08/22 05:49:49  Administrator
# Now Makefile generator understands to look into variables set in Makefile.
# This way Auto-Dependencies can be improved a little step and more important,
# the "TINOCOPY" functionality was added, such that I can copy files from
# elsewhere to the distribution and keep that files in sync.

#set -x

: getfile
getfile()
{
echo -n "$2"
cat "$1"
}

# Calculate MD5 sum
: md5
md5()
{
  eval "$1="
  [ -f "$2" ] || return
  eval "$1"='"$(getfile "$2" "$3" | fgrep -v @MD5TINOIGN@ | md5sum)"'
  eval "$1=\"\${$1%% *}\""
}

: md5copy
md5copy()
{
  md5 new "$1" "$3" || exit
  if [ ! -f "$2" ]
  then
	echo "$2 created."
	rm -f "$2"	# may be a dangling softlink
	getfile "$1" "$3" > "$2" || exit
  fi
  md5 old "$2"
  gawk -vOLD="$old" -vNEW="$new" -vNAME="$2" -vFROM="$1" '
BEGIN	{
	while ((getline < "Makefile.md5")>0)
		{
		md5=$1;
		sub(/[^ ]* /,"");
		has[$0]=md5;
		}
	close("Makefile.md5");

	if (OLD==NEW)
		ret=0;
	else if (has[NAME]==OLD)
		ret=1;
	else if (has[NAME]=="")
		{
		print "The integrity of " NAME " is unknown.";
		# Leave it in this state!
		exit(2);
		}
	else
		{
		print "The file " NAME " was edited."
		ret=2;
		}

	if (has[NAME]!=NEW)
		{
		has[NAME]=NEW
		for (a in has)
			print has[a] " " a > "Makefile.md5";
		}
	exit(ret);
	}' </dev/null
  case "$?" in
  0)	;;
  1)	echo "$2 updated."
	rm -f "$2"
	getfile "$1" "$3" > "$2" || exit
	;;
  2)	echo "The new version is at $1"
	echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
	false
	;;
  esac
}

here="`pwd`"
cd "$1" || exit
shift

gawk -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.d.proto" |
#tee Makefile.d~ |
make -f -

gawk -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.proto" >Makefile.~ || exit

md5copy Makefile.~ Makefile && rm -f Makefile.~

for tf
do
	md5copy "$tf" "${tf//\//_}" "#line 1 \"$tf\"
"
done
