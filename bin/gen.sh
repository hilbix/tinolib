#!/bin/bash
#
# Generate file from file with {{MACRO}} expansion
# and register it's checksum in $CHECKSUM_FILE

CHECKSUM_FILE=.generated.checksum

STDOUT() { local e=$?; printf '%q' "$1"; [ 1 -lt $# ] && printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR() { local e=$?; STDOUT "$@" >&2; return e; }
OOPS() { STDERR "OOPS:" "$@"; exit 23; }
x() { "$@"; }
o() { x "$@" || OOPS fail $?: "$@"; }
input() { "${@:2}" <"$1"; }
output() { "${@:2}" >"$1"; }
append() { "${@:2}" >>"$1"; }

checksum()
{
  local __CHECKSUM="$(git hash-object -- "$1")" || OOPS cannot hash "$1"
  o printf -vCHECKSUM '%q %q' "$__CHECKSUM" "${2:-$1}"
}

checksum_known()
{
  fgrep -qsx -- "$CHECKSUM" "$CHECKSUM_FILE"
}

checksum_write()
{
  checksum_known || o append "$CHECKSUM_FILE" echo "$CHECKSUM"
}

mvaway()
{
  local min=1 max=1
  # find a free $max
  while	[ -s "$1.~$max~" ]
  do
	let max+=max+$RANDOM%30
  done
  # try to find a lower free $max
  while	let mid=(min+max)/2
	[ $mid -gt $min ]
  do
	if [ -s "$1.~$mid~" ]
	then
		min=$mid
	else
		max=$mid
	fi
  done
  # $max wasn't there
  o mv -n "$1" "$1.~$max~"
}

template()
{
  o printf '/* DO NOT EDIT\n * GENERATED from %q\n */\n' "$1"
  let nr=0
  ok=false
  while IFS='' read -r line
  do
	let nr++
	if	[[ "$line" =~ ^(.*)\{\{([^}]*)\}\}(.*)$ ]]
	then
		o printf '#pragma MATCH %q\n' "${BASH_REMATCH[@]}"
		ok=false
	fi

	$ok || o printf '# %d "%q"\n' "$nr" "$1"
	o echo "$line"
	ok=:
  done
}

# check
test -s "$2" || OOPS invalid input file: "$2"

# generate
o output "$1.tmp" input "$2" template "$2"

# compare existing
if	[ -s "$1" ] && checksum "$1"
then
	x cmp -s "$1.tmp" "$1" &&
	o rm -f "$1.tmp" &&
	o touch "$1" &&
	checksum_write &&
	exit

	# cmp failed, so file differs
	checksum_known && o rm -f "$1"
fi

# record what we have done
checksum "$1.tmp" "$1"
checksum_write

# if target still exists, do a backup
# this replaces softlinks, too
[ -L "$1" ] || [ -e "$1" ] && mvaway "$1"
o mv -n "$1.tmp" "$1"

