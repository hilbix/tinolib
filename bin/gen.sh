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

replace()
{
  local cmd="${1%% *}"
  local arg="${1#"$cmd"}"
  local ARGS FILE="$2"
  read -ra ARGS <<<"${arg# }"
  declare -F -- "do-$cmd" >/dev/null ||
	{
	printf '#error unknown generator "%q"\n' "$cmd"
	return 1
	}
  o "do-$cmd" "${ARGS[@]}"
}

template()
{
  local name="${1//[^A-Z0-9a-z_]/_}"
  o printf '/* DO NOT EDIT, generated from %q\n' "$1"
  o printf ' *\n'
  o printf ' * This Works is placed under the terms of the Copyright Less License,\n'
  o printf ' * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.\n'
  o printf ' */\n'
  o printf '\n'
  o printf '#ifndef __INC_%s__\n' "$name"
  o printf '#define __INC_%s__\n' "$name"
  o printf '\n'
  let nr=0
  ok=false
  while IFS='' read -r line
  do
	let nr++
	if	[[ "$line" =~ ^(.*)\{\{([^}]*)\}\}(.*)$ ]]
	then
		o printf '# %d "%q"\n' "$LINENO" "$0"
#		o printf '#pragma MATCH %q\n' "${BASH_REMATCH[@]}"
		line=""
		replace "${BASH_REMATCH[2]}" "$1"
		ok=false
		line="${BASH_REMATCH[1]}$line${BASH_REMATCH[3]}"
	fi

	$ok || o printf '# %d "%q"\n' "$nr" "$1"
	o echo "$line"
	ok=:
  done
  o printf '#endif\n'
}

#### MACROS ####

do-ALLINCLUDES()
{
  for a in lib/*.h
  do
  	[ ".$FILE" = ".$a" ]  || printf '#include "%q"\n' "${a#*/}"
  done
}


#### Main ####
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

