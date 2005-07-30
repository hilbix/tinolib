#!/bin/sh
#
# $Header$
#
# Convenience script to setup new directory
#
# $Log$
# Revision 1.1  2005-07-30 16:13:24  tino
# Changes to enable setuptino.sh to newly setup an empty directory
#

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


if [ ! -d "$TARG" ]
then
	[ -e "$TARG" ] && fail "tino exists and is no directory"
	echo "Directory '$TARG' is missing"
	pressy "Create a softlink"
	ln -s "`dirname "$0"`" "$TARG" || fail "cannot softlink"
	echo "created directory $TARG"
fi

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
/\$Log$
/\Revision 1.1  2005-07-30 16:13:24  tino
/\Changes to enable setuptino.sh to newly setup an empty directory
/\]+\$$/	{
			logignore=$0;
			sub(/ *\$Log$
			sub(/ *\Revision 1.1  2005-07-30 16:13:24  tino
			sub(/ *\Changes to enable setuptino.sh to newly setup an empty directory
			sub(/ *\]+\$$/,"",logignore);
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
