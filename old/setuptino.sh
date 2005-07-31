#!/bin/sh
#
# $Header$
#
# Convenience script to setup new directory
#
# $Log$
# Revision 1.2  2005-07-31 12:43:13  tino
# First: CVS tags in AWK script now dequoted, such that CVS no more sees them.
# Second: Added Directory copy in case of directory setup
#
# Revision 1.1  2005/07/30 16:13:24  tino
# Changes to enable setuptino.sh to newly setup an empty directory

if [ 0 = $# ]
then
	echo "Usage: $0 target ...
" >&2
	exit 1
fi

TARG=tino

fail()
{
  echo "FAIL: $*" >&2
  exit 1
}


pressy()
{
  echo -n "$* [y/N]? "
  read ans &&
  case "$ans" in
  y)	return;;
  esac
  echo ".. ABORT"
  exit 1
}


if [ ! -e "$TARG" ]
then
	echo "Directory '$TARG' is missing"
	echo "If you this script to copy the directory,"
	echo "create an empty directory $TARG before calling this script!"
	pressy "Don't copy and instead create a softlink"
	ln -s "`dirname "$0"`" "$TARG" || fail "cannot softlink"
	echo "created directory $TARG"
elif [ -L "$TARG" ]
then
	if [ ! -d "$TARG/CVS" ] || ! cmp -s "$0" "$TARG/`basename "$0"`"
	then
		fail "$TARG does not point to the correct directory"
	fi
elif [ ! -d "$TARG" ]
then
	fail "$TARG is neither directory nor softlink"
elif [ ! -e "$TARG/`basename "$0"`" ] && [ ".$TARG" = ".`find $TARG -print`" ]
then
	echo "Directory '$TARG' is empty"
	pressy "Copy it from source"
	cp -rpP "`dirname "$0"`/." "$TARG/."
fi

cmp -s "$0" "$TARG/`basename "$0"`" ||
fail "Directory "$TARG" does not contain this script.  SAFTETY ABORT"

for a in "$TARG"/*.dist "$TARG"/.*.dist
do
	to="`basename "$a" .dist`"
	if [ ! -f "$to" ]
	then
		awk -vARG="$*" '
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

make -f Makefile.tino
make -f Makefile.tino
