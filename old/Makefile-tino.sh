#!/bin/bash
# $Header$
#
# Maintain Makefile.md5 file, a list of MD5 sums of files which are
# automatically generated (like the Makefile).
#
# $Log$
# Revision 1.4  2004-09-04 14:25:20  tino
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

# Calculate MD5 sum
: md5
md5()
{
  eval "$1="
  [ -f "$2" ] || return
  eval "$1"='"$(fgrep -v @MD5TINOIGN@ "$2" | md5sum)"'
  eval "$1=\"\${$1%% *}\""
}

# Fetch MD5 sum from MD5 list
# Easy (stupid) implementation
: md5get
md5get()
{
  while read $1 name
  do
	[ ".$2" = ".$name" ] && return
  done < Makefile.md5
  eval $1=
  return 1
}

# Compare if file matches MD5 sum
: md5cmp
md5cmp()
{
  if [ -n "$was" ]
  then
	echo "The integrity of $2 is unknown."

  elif	md5 cmp2 "$2" || exit
	[ ".$cmp2" = ".$1" ]
  then
	return 0
  else
	echo "The file $2 was edited."
  fi
  echo "The new version is at $3"
  return 1
}

# Insert/Replace MD5 sum in MD5 list
: md5set
md5set()
{
  awk -vMD5="$1" -vNAME="$2" '
BEGIN	{
	while ((getline < "Makefile.md5")>0)
		{
		md5=$1
		sub(/[^ ]* /,"");
		has[$0]=md5
		}
	close("Makefile.md5")

	has[NAME] = MD5
	for (a in has)
		print has[a] " " a > "Makefile.md5"
	}' </dev/null || exit
}

# This version has too much overhead
# in case a lot of is to do
: md5copy
md5copy_old()
{
  md5 new "$1" || exit  
  md5get was "$2"

  ret=:
  if cmp -s "$1" "$2"
  then
	echo "$2 is up to date."
  elif [ ! -f "$2" ] || md5cmp "$was" "$2" "$1"
  then
	echo "$2 updated."
	rm -f "$2"
	cp -f "$1" "$2" || exit
  else
	ret=false
  fi
#  echo "was=$was new=$new"
  [ ".$was" != ".$new" ] && md5set "$new" "$2"
  $ret
}

: md5copy
md5copy()
{
  md5 new "$1" || exit
  if [ ! -f "$2" ]
  then
	echo "$2 created."
	rm -f "$2"	# may be a dangling softlink
	cp -f "$1" "$2"
  fi
  md5 old "$2"
  awk -vOLD="$old" -vNEW="$new" -vNAME="$2" -vFROM="$1" '
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
	cp -f "$1" "$2" || exit
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

awk -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.d.proto" |
make -f -

awk -vSRC="$here" -f"$here/Makefile.awk" Makefile.tino "$here/Makefile.proto" >Makefile.~ || exit

md5copy Makefile.~ Makefile && rm -f Makefile.~

for tf
do
	md5copy "$tf" "${tf//\//_}"
done
