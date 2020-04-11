#!/bin/bash
#
# Clean files if they math $CHECKSUM_FILE

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

for a
do
	[ -L "$a" ] || [ -e "$a" ] || continue
	checksum "$a"
	checksum_known && rm -f "$a" || mvaway "$a"
done

