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


########################################################################
# HELPERS
########################################################################

STDOUT() { local e=$?; printf '%q' "$1"; [ 1 -lt $# ] && printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR() { local e=$?; STDOUT "$@" >&2; return $e; }
OOPS() { n="$(caller "$1")"; shift; printf '#E#%s#%d#0#%s#\n' "$ME" "${n%% *}" "$*" >&2; STDERR "OOPS:" "$@"; exit 23; }
x() { "$@"; }
o() { x "$@" || OOPS 1 fail $?: "$@"; }
input() { "${@:2}" <"$1"; }
output() { "${@:2}" >"$1"; }
append() { "${@:2}" >>"$1"; }

DEBUG() { STDERR DEBUG  "$@"; }

checksum()
{
  local __CHECKSUM="$(git hash-object -- "$1")" || OOPS 0 cannot hash "$1"
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

realpath()
{
  "$(dirname -- "$0")/realpath.sh" "$1"
}

########################################################################
# ASSOC
########################################################################
# Sadly BASH on MacOS does not know declare -A, so emulate it.
#
# This works as follows:
#
# Aset assoc key val1..	Set the variables into key of the given assoc
# Aget assoc key var1..	Get the variables from key of the given assoc
#
# For speedup assoc/var are transscripted into environment variables below __ASSOC__*
# __ASSOC_A_${assoc}=(keys)
# __ASSOC_K_${assoc}_${key}=N	# number of key in above array, if missing: deleted
# __ASSOC_V0_${assoc}_${key}=var1
# __ASSOC_V1_${assoc}_${key}=var2 and so on
# key is escaped as follows:
# - A-Za-z0-1 are used as is
# - everything else becomes _N_ where N is the character code

# Sets __ASSOC_K to a shell compatible name created out of given assoc+key
# assoc must be some unique shell compatible name without "_"
: __Akey assoc key
__Akey()
{
  __ASSOC_K=("$1")
  local a="$2" b

  while	b="${a%%[^A-Za-z0-9]*}"
	__ASSOC_K+=("$b")
	a="${a#"$b"}"
	[ -n "$a" ]
  do
	printf -vb '%x' "'$a"
	__ASSOC_K+=("$c")
	a="${a#?}"
  done

  a="${__ASSOC_K[*]}"
  __ASSOC_K="${a// /_}"
#  a="__ASSOC_K_$__ASSOC_K"
#  [ -n "${!a}" ] && return
#
#  declare -g -a "__ASSOC_K_$1"
#  eval b="\${#__ASSOC_K_$1[@]}"
#  declare -g -a "$a=$b"
#  eval "__ASSOC_K_$1+=(\"\$2\")"
}

# __Akey must have been called before
: __Aget 0 var..
__Aget()
{
  __ASSOC_V="__ASSOC_V$1_$__ASSOC_K"
  printf -v"$2" '%s' "${!__ASSOC_V}"	# do you have any better idea?
  [ 2 -ge "$#" ] || __Aset $[$1+1] "${@:3}"
}

: Aset assoc key val..
Aset()
{
  local a n

  __Akey "$1" "$2"
  shift 2
  n=0
  for a
  do
	printf -v"__ASSOC_V${n}_$__ASSOC_K" '%s' "$a"	# do you have any better idea?
	let n++
  done
}

: __Aget assoc key var..
Aget()
{
  __Akey "$1" "$2"
  __Aget 0 "${@:3}"
}


########################################################################
# transform {{macro}} parts into stage-commands
########################################################################

# output something coming from SRC
: putsrc "data"
putsrc()
{
# DEBUG putsrc "$@"
  o printf 'cmd line src %d %q\n' "$lineno" "$src"
  o printf 'cmd src %q\n' "$1"
}

# output something coming from this script
: putout format args..
putout()
{
# DEBUG putout "$@"
  local o n="$(caller)"
  o printf 'cmd line out %d %q\n' "${n%% *}" "$ME"
  o printf -vo -- "$@"
  o printf 'cmd out %q\n' "$o"
}

# output a special command for MACRO processing
: putcmd cmd args..
putcmd()
{
# DEBUG putcmd "$@"
  local o n="$(caller)"
  o printf 'cmd line out %d %q\n' "${n%% *}" "$ME"
  o printf 'cmd'; printf ' %q' "$@"; printf '\n'
}

# Sadly regex do not work here.
# Regex:	^(.*)aa(.*?)kk(.*)$
# String:	XaabcaabkkkkY
# We either see 'X' 'bcaab' 'kk' or 'Xaabc' 'bkk' 'Y'
# but both are plain wrong, as we want to see just 'Xaabc' 'b' 'kkY' to process 'b'.
expand()
{
  local a b c

  b="${1%%'{{'*}"
  a="${1#"$b"}"
  [ -z "$b" ] || putsrc "$b"
  [ -z "$a" ] && return		## line without macro, $b is full line

  b="${a##*'}}'}"
  a="${a%"$b"}"
  [ -z "$a" ] && putout '#error "missing }}"' && putsrc "$b" && return

  # $a == "{{CMD args..}}" but perhaps must be expanded
  a="${a#'{{'}";
  a="${a%'}}'}";

  putcmd macro "$a"
  expand "$a"		# recurse into sub-macros
  putcmd end "$a"

  [ -z "$b" ] || putsrc "$b"
}

include()
{
  local lineno=0 line

  while	let lineno++
	IFS='' read -r line
  do
#	DEBUG "$src" "$lineno" "$line"
	expand "$line"
  done
  putcmd ok
}

process()
{
  local src="$(realpath "$2")"

  stage 1 include
}

template()
{
  local name="${1//[^A-Z0-9a-z_]/_}"

  putout '/* DO NOT EDIT, generated from %q' "$1"
  putout ' *'
  putout ' * This Works is placed under the terms of the Copyright Less License,'
  putout ' * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.'
  putout ' */'
  putout ''
  putout '#ifndef __INC_%s__' "$name"
  putout '#define __INC_%s__' "$name"
  putout ''

  process "$1"

  putout '#endif'
}

########################################################################
# STAGE 1: macro processing
########################################################################

S1-line()
{
# DEBUG stage1 line "$@"
  o printf 'cmd line %s %d %q\n' "$@"
}

S1-src()
{
  DEBUG stage1 src "$@"
  o printf 'cmd src %q\n' "$1"
}

S1-out()
{
# DEBUG stage1 out "$@"
  o printf 'cmd out %q\n' "$1"
}

S1-ok()
{
# DEBUG stage1 ok "$@"
  o printf 'cmd ok\n'
  STAGE_OK=1
}




########################################################################
# STAGE 0: the output stage
########################################################################

lastS0line=

# line type NR file
S0-line()
{
# DEBUG stage 0: line "$@"

  local nr=0

  Aset last "$1" "$3"
  Aget lines "$3" nr
  [ ".$2" = ".$nr" ] && return
  Aset lines "$3" "$2"
  if [ -n "$nr" -a "$[nr+1]" = "$2" ]
  then
	o printf '\n'
  else
	o printf '# %d "%s"\n' "$2" "$3"
  fi
}

S0-src()
{
# DEBUG stage 0: src "$@"

  local file nr

  Aget last src file
  Aget lines "$file" nr
  Aset lines "$file" $[nr+1]
  echo "$@"
}

S0-out()
{
# DEBUG stage 0: out "$@"
  local file nr

  Aget last out file
  Aget lines "$file" nr
  Aset lines "$file" $[nr+1]
  echo "$@"
}

S0-ok()
{
# DEBUG stage 0: ok "$@"
  STAGE_OK=0
}

cmd()
{
  case "$1" in
  (*[^a-z]*)	OOPS 0 internal error: illegal cmd in $STAGE: "$@";;
  esac
  x declare -F -- "S$STAGE-$1" >/dev/null ||
  OOPS 0 stage $STAGE error: undefined cmd: "$@"
  "S$STAGE-$1" "${@:2}"
}

: stage NR cmd args..
stage()
{
  STAGE="$1"
  STAGE_OK=
  DEBUG stage "$@"
  . <("${@:2}")

  [ ".$1" = ".$STAGE_OK" ] ||
  OOPS 0 stage $STAGE error: missing cmd ok
}







getlines()
{
  lines=()
  while	getline "{{$1}}" || OOPS 0 not found: "{{$1}}" "${@:2}"
	[ "{{$1}}" != "$line" ]
  do
	lines+=("$line")
  done
  line=""	# remove {{$1}}
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
[ -s "$2" ] || OOPS 0 invalid input file: "$2"

# generate
o output "$1.tmp" input "$2" stage 0 template "$2"

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

