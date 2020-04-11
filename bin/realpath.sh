#!/bin/bash

get()
{
  local _v

  _v="$("${@:2}" && echo x)" || exit
  _v="${_v%x}"		# remove the 'x' from echo x
  _v="${_v%$'\n'}"	# remove a single LF only

  printf -v "$1" '%s' "$_v"
}

realpath()
{
  local p="$1" f

  pushd . >/dev/null

  while	get f dirname -- "$p" &&
	cd -- "$f" &&
	get f basename -- "$p" &&
	[ -L "$f" ]
  do
	get p readlink -- "$f"
  done
  if [ -d "$f" ]
  then
	cd -- "$f" && pwd -P
  else
	get p pwd -P
	echo "$p/$f"
  fi

  popd >/dev/null
}

for a
do
	realpath "$a"
done

