#!/usr/bin/perl
# $Header$
#
# Copyright (C)2006 by Valentin Hilbig, webmaster@scylla-charybdis.com
#
# This file is free software according to the GNU GPL v2 or higher.
# Use at own risk!
#
# $Log$
# Revision 1.1  2006-03-22 12:17:07  tino
# This version is nearly untested
#

use File::Basename;
use File::Spec::Functions;

if ($#ARGV < 1) {
  print STDERR "Usage: $0 prefix directory file...\n";
  print STDERR "\tThe idea is to get a snippet from a rotating logfile:\n";
  print STDERR "\tAll files are sorted by their name, except for the last file.\n";
  print STDERR "\tThe last given file stays the last file, ever.\n";
  print STDERR "\tThen hunt for (possibly) empty list matching prefix and\n";
  print STDERR "\tcat the files just before and behind, too, in sequence.\n";
  exit(1);
}

my $prefix=@ARGV[0];
shift @ARGV;

my %list;
my $last="";

for (@ARGV) {
  if ( -d $_ ) {
    my $dir = $_;
    die "cannot open dir $dir" unless opendir SRC,$dir;
    my @files = grep { -f "$dir/$_" } readdir(SRC);
    die "dir error $dir" unless closedir(SRC);
    $list{$_}=catfile($dir,$_) for (@files);
  } elsif ( -f $_ ) {
    $list{basename($last)}=$last if $last ne "";
    $last	= $_;
  }
}

sub cat {
  my ($fn)=@_;
#  print STDERR "out $fn\n";
  die "cannot read $fn" unless open INP, $fn;
  print $_ while (<INP>);
  die "cannot close $fn" unless close INP;
}

$was	= "";
for (sort keys %list) {
  my $fi=$_;
#  print STDERR "in $fi\n";
  if ($fi lt $prefix) {
    $was	= $list{$fi};
    next;
  }
  if ($was ne "") {
    cat($was);
    $was="";
  }
  if (substr($fi,0,length($prefix)) ne $prefix) {
    $last	= $list{$fi};
    last;
  }
  cat($list{$fi});
}
#print STDERR "last\n";
cat($last) if $last ne "";
