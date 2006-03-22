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
# Revision 1.1  2006-03-22 10:04:27  tino
# created
#

if ($#ARGV < 2) {
  print STDERR "Usage: $0 prefix .suffix name...\n";
  print STDERR "\tRename the name to prefixYYMMDD-HHMMSS-NNN.suffix\n";
  print STDERR "\tThis works on files, diectories or special names\n";
  print STDERR "\tCaution: It moves the files to the current directory.\n";
  print STDERR "\t\tThis may or may not work across drives.\n";
  print STDERR "\tIt skips missing names but it stops on the first error.\n";
  exit(1);
}

my $prefix=@ARGV[0];
shift @ARGV;
my $suffix=@ARGV[0];
shift @ARGV;

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

$count=0;
for (@ARGV) {
  if ( ! -e $_ ) {
    print "skipped missing $_\n";
  } else {
    $out = sprintf("%s%04d%02d%02d-%02d%02d%02d-%03d%s", $prefix, 1900+$year, 1+$mon, $mday, $hour, $min, $sec, $count, $suffix);
    die "$_: cannot rename to $out" unless rename($_,$out);
    print "renamed $out from $_\n";
    $count++;
  }
}
