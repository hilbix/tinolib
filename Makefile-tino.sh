#!/bin/bash
#
# Maintain Makefile.md5 file, a list of MD5 sums of files which are
# automatically generated (like the Makefile).
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

#set -x

GAWK="`which gawk`" || GAWK="`which awk`" || exit

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
  "$GAWK" -vOLD="$old" -vNEW="$new" -vNAME="$2" -vFROM="$1" '
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
here="${here#`pwd`/}"

# If this GAWK fails, it outputs nothing, so "make" does nothing
"$GAWK" -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.d.proto" |
#tee Makefile.d~ |
make -f -

"$GAWK" -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.proto" >Makefile.~ ||
{ echo $'\n###\n'"### Your $GAWK is no gawk, please install gawk!"$'\n###\n'; exit 1; }

md5copy Makefile.~ Makefile && rm -f Makefile.~

for tf
do
	md5copy "$tf" "${tf//\//_}" "#line 1 \"$tf\"
"
done
