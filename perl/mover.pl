#!/usr/bin/perl
# $Header$
#
# Copyright (C)2006 by Valentin Hilbig, webmaster@scylla-charybdis.com
#
# This file is dedicated into the Public Domain
# as long as you do not claim a copyright on this.
# (You can remove my copyright but you must make sure that nobody adds another.)
#
# $Log$
# Revision 1.1  2006-03-22 10:41:17  tino
# added
#

use File::Copy;

if ($#ARGV != 2) {
  print STDERR "$#ARGV Usage: $0 destdir sourcedir regexp\n";
  print STDERR "\tMoves matching files from sourcedir to destdir.\n";
  print STDERR "\tIf it cannot rename it moves by copying.\n";
  print STDERR "\tCAUTION: Existing destinations are overwritten!\n";
  print STDERR "\t\tThe last arg is no filename, it's a regexp!\n";
  print STDERR "\t\tSo escape dots, like in '\\.log\$' (files ending in .log)\n";
  print STDERR "\t\tAlso a*.* is wrong, correct is '^a.*\\..*\$' (or '^a.*\\.')\n";
  print STDERR "\tThis is not atomic, see renamer.pl for atomic actions.\n";
  exit(1);
}

my $dest=@ARGV[0];
my $source=@ARGV[1];
my $match=@ARGV[2];

die "dir not found $source" unless opendir SRC,$source;
my @files = grep { /$match/ && -f "$source/$_" } readdir(SRC);
die "dir error $source" unless closedir(SRC);

for (@files) {
  die "cannot move $_ from $source to $dest" unless move("$source/$_","$dest/$_");
  print "$_\n";
}
