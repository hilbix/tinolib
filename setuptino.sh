#!/bin/bash
#
# Convenience script to setup new directory
#
# Copyright (C)2005-2013 Valentin Hilbig, webmaster@scylla-charybdis.com
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

TARG=tino
DIET=diet
SRC="`dirname "$0"`"
ME="`basename "$0"`"

DIET=	# currently not supported

fail()
{
  echo "FAIL: $*" >&2
  exit 1
}

abort()
{
  echo ".. ABORT"
  exit 1
}

pressy()
{
  echo -n "$* [y/N]? "
  read ans &&
  case "$ans" in
  y)	return;;
  esac
  abort
}

if [ ! -e "$TARG" ]
then
	echo "Directory '$TARG' is missing"
	echo "1) Create the directory (contained tinolib)"
	echo "2) Create a softlink (optional tinolib)"
	echo -n "What to do? "
	read ans
	case "$ans" in
	1)	mkdir "$TARG" || fail "cannot create directory $TARG";;
	2)	ln -s "$SRC" "$TARG" || fail "cannot create softlink $TARG";;
	*)	abort;;
	esac
	echo "created directory $TARG"
fi

if [ -L "$TARG" ]
then
	{ [ -d "$TARG/.git" ] && cmp -s "$0" "$TARG/$ME"; } ||
		fail "$TARG does not point to the correct directory"
elif [ ! -d "$TARG" ]
then
	fail "$TARG is neither directory nor softlink"
elif [ ! -e "$TARG/$ME" ] && [ ".$TARG" = ".`find $TARG -print`" ]
then
	echo "Directory '$TARG' is empty"
	pressy "Checkout?"
	cp -r "$SRC/.git" "$TARG/"
	( cd "$TARG"; git checkout -- .; )
	git submodule add "$(git --git-dir="$TARG/.git" config --get remote.origin.url)" "$TARG"
fi

[ -n "$DIET" ] &&
if [ -d $DIET ] && [ ".$DIET" = ".`find $DIET -print`" ]
then
	[ -e "$SRC/../$DIET/tinodiet.sh" ] ||
	fail "diet option exists but $SRC/../$DIET is not a proper source"

	echo "Directory '$DIET' is empty"
	pressy "Copy it from source"
	cp -rpP "$SRC/../$DIET/." "$DIET/."
fi

cmp -s "$0" "$TARG/$ME" ||
fail "Directory "$TARG" does not contain this script.  SAFTETY ABORT"

for a in "$TARG"/*.dist "$TARG"/.*.dist
do
	to="`basename "$a" .dist`"
	if [ ! -f "$to" ]
	then
		if [ -z "$*" ]
		then
			set -- "`basename "$PWD" | sed 's/\.[A-Z]*$//'`"
			echo
			echo "Missing argument autoguessed as $*"
			pressy "Is it correct to create entries for this source"
		fi
		gawk -vARG="$*" '
logignore!="" && substr($0,1,length(logignore))==logignore {
			next
			}
			{
			logignore="";
			}
/\$[L]og: [^$]+\$$/	{
			logignore=$0;
			sub(/ *\$[L]og: [^$]+\$$/,"",logignore);
			}
/^!NAME!/		{
			split(ARG,a);
			for (b in a)
				{
				o=$0;
				gsub(/!NAME!/,a[b],o);
				print o;
				}
			next;
			}
			{
			gsub(/!NAME!/,ARG);
			print gensub(/\$([A-Z][a-z]+): [^$]+\$/, "$\\1$", "g");
			}
' "$a" > "$to" &&
		echo "created file $to"
	fi
done

echo

# This is no stutter
make -f Makefile.tino
make -f Makefile.tino

