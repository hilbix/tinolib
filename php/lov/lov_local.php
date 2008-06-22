<?
# $Header$
#
# For a suitable lov_local.php see lov_local.php.dist
# This here is a wrapper to find the correct one.
#
# $Log$
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin
#

$lovinc	= dirname($lovinc);
include("$lovinc/lov_local.php");
?>
