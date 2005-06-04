<?
# $Header$
#
# This is part of tinolib.
# Copyright (C)2005 by Valentin Hilbig
#
# Tinolib may be copied according to GNU GPL v2 or higher
# (Yes, GPL for now, not yet LGPL, sorry.)
#
# $Log$
# Revision 1.1  2005-06-04 17:58:35  tino
# added
#

function o($s)
{
  echo htmlentities($s);
}
function u($s)
{
  echo rawurlencode($s);
}

function h($s,$a)
{
  $s	= split("%", $s);
  while (list(,$v)=each($s))
    {
      echo $v;
      while (list(,$v)=each($s))
        {
          if ($v=='')
	    {
              echo "%";
	      break;
	    }
	  echo htmlentities($a[substr($v,0,1)]);
	  echo substr($v,1);
        }
    }
}

function list_options($a,$def="")
{
  reset($a);
  while (list($k,$v)=each($a))
    if ($k==$def)
      h("<OPTION VALUE='%0' SELECTED>%1</OPTION>\n", array($k,$v));
    else
      h("<OPTION VALUE='%0'>%1</OPTION>\n", array($k,$v));
}
?>
