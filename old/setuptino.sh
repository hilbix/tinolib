#!/bin/sh
#
# $Header$
#
# Convenience script to setup new directory
#
# $Log$
# Revision 1.5  2005-09-26 18:35:48  tino
# improved setuptino.sh
#
# Revision 1.4  2005/08/14 02:17:25  tino
# forgot a . on copy of DIET
#
# Revision 1.3  2005/08/14 02:12:23  tino
# diet detection added to setup
#
# Revision 1.2  2005/07/31 12:43:13  tino
# First: CVS tags in AWK script now dequoted, such that CVS no more sees them.
# Second: Added Directory copy in case of directory setup
#
# Revision 1.1  2005/07/30 16:13:24  tino
# Changes to enable setuptino.sh to newly setup an empty directory

TARG=tino
DIET=diet

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
	2)	ln -s "`dirname "$0"`" "$TARG" || fail "cannot create softlink $TARG";;
	*)	abort;;
	esac
	echo "created directory $TARG"
fi

if [ -L "$TARG" ]
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

if [ -d $DIET ] && [ ".$DIET" = ".`find $DIET -print`" ]
then
	[ -e "`dirname "$0"`/../$DIET/tinodiet.sh" ] ||
	fail "diet option exists but `dirname "$0"`/../$DIET is not a proper source"

	echo "Directory '$DIET' is empty"
	pressy "Copy it from source"
	cp -rpP "`dirname "$0"`/../$DIET/." "$DIET/."
fi

cmp -s "$0" "$TARG/`basename "$0"`" ||
fail "Directory "$TARG" does not contain this script.  SAFTETY ABORT"

for a in "$TARG"/*.dist "$TARG"/.*.dist
do
	to="`basename "$a" .dist`"
	if [ ! -f "$to" ]
	then
		if [ -z "$*" ]
		then
			echo "!!! $to: cannot create file, as you gave no arguments" >&2
			continue
		fi
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
