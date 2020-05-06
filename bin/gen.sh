#!/bin/bash
#
# Generate file from file with {{MACRO}} expansion
# and register it's checksum in $CHECKSUM_FILE
#
# Works as follows:
# - Read input
# - Transform input into a shell script which outputs the input again with macros expanded
# - run this shell script
# - save output into file
# - register checksum and overwrite target
#
# MACROs:
#
# {{$var}} is an environment variable as in shell
# {{SET var}}content{{.}} sets environment variable {{$var}} to the content
# {{MAP map sep}} defines a new map until {{.}} is found, def sep is TAB
# {{LOOP map..}} loops over the given MAPs
# {{0}} is the full line of a map
# {{1}} {{2}} and so on are the columns of the line of a map
# {{0.0}} {{0.1}} and so on are the same as {{0}} {{1}} {{2}} and so on
# {{1.0}} and so on take the map of the outer loop
# {{IF sth}}true{{else}}false{{.}} use false if something is empty or ".", else true
# {{GLOB pattern..}} outputs pattern, one each line, nothing if not found

CHECKSUM_FILE=.generated.checksum

STDOUT() { local e=$?; printf '%q' "$1"; [ 1 -lt $# ] && printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR() { local e=$?; STDOUT "$@" >&2; return $e; }
OOPS() { STDERR "OOPS:" "$@"; exit 23; }
x() { "$@"; }
o() { x "$@" || OOPS fail $?: "$@"; }
input() { "${@:2}" <"$1"; }
output() { "${@:2}" >"$1"; }
append() { "${@:2}" >>"$1"; }

DEBUG() { STDERR DEBUG  "$@"; }

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
  case "$cmd" in
  ([A-Z]*)	replace-cmd;;
  ([0-9]*)	replace-loop;;
  (*)		false;;
  esac ||
  o put '' "$LINENO" '#error removed unknown sequence: "{{%q}}"' "$3"

  line="$2$line$4"
}

replace-cmd()
{
  declare -F -- "do-$cmd" >/dev/null && o "do-$cmd" "${args[@]}"
}

replace-loop()
{
  local nr="${cmd##*.}"
  local depth="${cmd%"$nr"}"
  local parm


  depth="${depth:-0}"
  case "$nr$depth" in
  (*[^0-9]*)	return 1;;
  esac

  DEBUG "$nr" "$depth"

  [ 0 -le "$depth" -a "${#loops[@]}" -gt "$depth" ] || return

  line="${loops[$depth]}"
  [ 0 = "$nr" ] && return

  o put '' "$LINENO" '===HERE=== %q' "$line"

  IFS="${seps["$mapped"]:-$'\t'}" read -ra parm <<<"$line"

  line=""
  let nr-- || return
  [ 0 -le "$nr" -a "${#parm[@]}" -gt "$nr" ] || return

  line="${parm[$nr]}"
}

realpath()
{
  "$(dirname -- "$0")/realpath.sh" "$1"
}

puts()
{
  o printf 'line "$SRC" %d\n' "$lineno"
  o printf 'out'; printf ' %q' "$@"; printf '\n'
}

putm()
{
  o printf 'line %q %q\n' "$ME" "$1"
  shift
  o printf 'out'; printf ' %q' "$@"; printf '\n'
}

# Sadly regex do not work here.
# Regex:	^(.*)aa(.*?)kk(.*)$
# String:	XaabcaabkkkkY
# We either see 'X' 'bcaab' 'kk' or 'Xaabc' 'bkk' 'Y'
# but both are plain wrong, as we want to see just 'Xaabc' 'b' 'kkY' to process 'b'.
expand()
{
  local a b c

  b="${a%%'{{'*}"
  a="${a#"$b"}"
  [ -z "$b" ] || puts "$b"
  [ -z "$a" ] && return		## line without macro

  b="${a##*'}}'}"
  a="${a%"$b"}"
  [ -z "$a" ] && putm "$LINENO" '#error "%missing }}"' && puts "$b" && return

  # $a == "CMD args.." but perhaps must be expanded
  while [ -n "$a" ]
  do
	c="${1##*'}}'}"

  [ -z "$b" ] || puts "$b"
}

lineno=0
getl()
{

  let lineno++
  IFS='' read -r line || return
  [ ".$1" = ".$line" ]  && return

  expand "$line"
}


  while	[ -n "$line" ]
  	a="${line%%'{{'*}"
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

eof=0
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

# MacOS has no assoc arrays, sigh
maps=()		# NR->map deserialization
seps=()		# NR->separator used
mapname=()	# NR->name
mapped=()	# (NR name)

getmap()
{
  local a

  [ ".${mapped[1]}" = ".$1" ] && return
  map=()
  mapped=("${#maps[@]}" "$1")
  mapname["${#maps[@]}"]="$1"

  for a in "${!maps[@]}"
  do
	[ ".$1" = ".${mapname[$a]}" ] || continue
	eval "${maps["$a"]}"
	mapped=("$a" "$1")
	break
  done
}

setmap()
{
  maps["$mapped"]="$(declare -p map)"	# do you have any better idea?
  seps["$mapped"]="$2"
}

# {{MAP map sep}}
# mapdata
# {{.}}
# sep is the column separator, default: TAB
do-MAP()
{
  local lines mapped=() map=()

  getmap "$1"
  getlines . 'for' MAP started at line "$lineno"
  map+=("${lines[@]}")
  setmap
}

loops=()
loopd=()

# {{LOOP map1}}
# {{LOOP map0 map0..}}
# lines to output
# {{.}}
# {{.}}
#
# {{0}} {{1}} {{2}} same as {{0.0}} {{0.1}} {{0.2}} and so on
# {{0.0}}=map0 line, {{0.1}}=first column, {{0.2}}=2nd column
# {{1.0}}=map1 line, {{0.1}}=first column, {{0.2}}=2nd column
do-LOOP()
{
  local lines mapped=() map=() loops=("" "${loops[@]}") loopd=("" "${loopd[@]}") a b parm all=()
  for a
  do
	getmap "$a"
	loopd[0]="${seps[$mapped]}"
	for b in "${ARGS[@]}"
	do
		DEBUG loop "$a" "$b"
		loops[0]="$b"
		getlines . 'for' LOOP started at line "$lineno"
	done
	all+="${lines[@]}"
  done
  printf -vline '%s\n' "${all[@]}"
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

