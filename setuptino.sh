#!/bin/bash
#
# Convenience script to setup new directory
#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
#
# There are no plans to support .git/modules files.

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

need()
{
for needs
do
	which "$needs" >/dev/null || fail "missing tool '$needs'"
done
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

[ -d "$SRC/.git" ] || fail "missing .git directory in $SRC (.git files are not yet supported)"

need git gawk make find
if	! git status >/dev/null
then
	pressy "GIT repo missing.  Create"
	git init
elif [ ! -e .git ]
then
	pressy "You are not in the top level GIT directory, continue"
fi

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
	(
	need readlink mountpoint
	here="$(readlink -e "$TARG")"
	while	now="$(readlink -e .)" && [ ! -e .git ]
	do
		if	mountpoint .
		then
			now=OOPS
			break
		fi
		[ / = "$PWD" ] && fail "mountpoint does not work"
		cd ..
	done >/dev/null
	case "$here" in
	"$now"/*)	git submodule add "$(git --git-dir="$here/.git" config --get remote.origin.url)" "${here#$now/}";;
	*)		pressy "Cannot find top level GIT directory.  Continue anyway";;
	esac
	) || abort
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
	[ -f "$a" ] || continue
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

