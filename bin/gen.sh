#!/bin/bash
#
# Generate file from file with {{MACRO}} expansion
# and register it's checksum in $CHECKSUM_FILE

CHECKSUM_FILE=.generated.checksum

STDOUT() { local e=$?; printf '%q' "$1"; [ 1 -lt $# ] && printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR() { local e=$?; STDOUT "$@" >&2; return $e; }
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

: replace-line file prefix cmdargs.. suffix
replace-line()
{
  local cmd="${3%% *}" args
  local ARGS="${3#"$cmd"}"
  local FILE="$1"
  ARGS="${ARGS# }"
  read -ra args <<<"$ARGS"

  line=""
  if	declare -F -- "do-$cmd" >/dev/null
  then
	o "do-$cmd" "${args[@]}"
  else
	o put '' "$LINENO" '#error removed unknown sequence: "{{%q}}"' "$3"
  fi
  line="$2$line$4"
}

realpath()
{
  "$(dirname -- "$0")/realpath.sh" "$1"
}

getline()
{
  local a b c

  let lineno++
  IFS='' read -r line || return
  [ ".$1" = ".$line" ]  && return

  # Sadly regex do not work here.
  # Regex:	^(.*)aa(.*?)kk(.*)$
  # String:	XaabcaabkkkkY
  # We either see 'X' 'bcaab' 'kk' or 'Xaabc' 'bkk' 'Y'
  # but both are plain wrong, as we want to see just 'Xaabc' 'b' 'kkY' to process 'b'.
  while	a="${line##*'{{'}"
	b="${a%%'}}'*}"
	[ ".$line" != ".$b" ]
  do
	if	[ ".$line" = ".$a" ] || [ ".$a" = ".$b" ]
	then
		put '' "$LINENO" '#error unmatched "{{" and "}}"'
		break
	fi

	replace-line "$1" "${line%"{{$a"}" "$b" "${a#"$b}}"}"
  done
  :
}

getlines()
{
  lines=()
  while	getline "{{$1}}" || OOPS not found: "{{$1}}" "${@:2}"
	[ "{{$1}}" != "$line" ]
  do
	lines+=("$line")
  done
  line=""	# remove {{$1}}
}

template()
{
  local name="${1//[^A-Z0-9a-z_]/_}"
  local src="$(realpath "$1")"

  put '' "$LINENO" '/* DO NOT EDIT, generated from %q' "$1"
  put '' "$LINENO" ' *'
  put '' "$LINENO" ' * This Works is placed under the terms of the Copyright Less License,'
  put '' "$LINENO" ' * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.'
  put '' "$LINENO" ' */'
  put '' "$LINENO" ''
  put '' "$LINENO" '#ifndef __INC_%s__' "$name"
  put '' "$LINENO" '#define __INC_%s__' "$name"
  put '' "$LINENO" ''
  lineno=0
  while	getline
  do
	put "$src" '' '%s' "$line"
  done
  o put '' "$LINENO" '#endif'
}

putlast=
putfile=

: put file line data
put()
{
  local file="${1:-$ME}"
  local nr="${2:-$lineno}"
  local out tmp

  if	[ ":$putfile:$putlast" != ":$file:$nr" ]
  then
	putfile="$file"
	putlast="$nr"
	o printf '# %d "%q"\n' "$nr" "$file"
  fi
  o printf -vout -- "${@:3}"
  while	let putlast++
	tmp="${out%%$'\n'*}"
	o printf '%s\n' "$tmp"
	out="${out#"$tmp"}"
	[ -n "$out" ]
  do
	out="${out#$'\n'}"
  done
}


#### MACROS ####

do-LF()
{
  line=$'\n'
}

declare -A maps seps

# {{MAP map sep}}
# mapdata
# {{END}}
# sep is the column separator, default: TAB
do-MAP()
{
  local lines map=()

  eval "${maps["$1"]}";			# does nothing if empty
  getlines END for MAP started at line "$lineno"
  map+=("${lines[@]}")
  maps["$1"]="$(declare -p map)"	# do you have any better idea?
  seps["$1"]="$2"
}

do-GLOB()
{
  local a b

  for a
  do
	for b in $a
	do
		[ -e "$b" ] && lines+=("$b")
	done
  done
  printf -vline '%s\n' "${lines[@]}"
}

# {{LOOP map string}}
# #0# is the map line
# #1# is the first column
# #2# is the 2nd column and so on
do-LOOP()
{
  local lines map=() a parm s
  eval "${maps["$1"]}"			# does nothing if empty

  ARGS="${ARGS#"$1 "}"
  for a in "${map[@]}"
  do
	IFS="${seps["$1"]:-$'\t'}" read -ra parm <<<"$a"
	parm=("$a" "${parm[@]}")
	s="$ARGS"
	while	[[ "$s" =~ ^(.*)#([0-9]*)#(.*)$ ]]
	do
		s="${BASH_REMATCH[1]}${parm[${BASH_REMATCH[2]}]}${BASH_REMATCH[3]}"
	done
	lines+=("$s")
  done
  line="${lines[*]}"
}


#### Main ####

ME="$(realpath "$0")"
[ -s "$2" ] || OOPS invalid input file: "$2"

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

