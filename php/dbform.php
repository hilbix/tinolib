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

class db_form
  {
    var	$form;

    function db_form($table, $db)
      {
	var	$q;

	$q	= new db_query($db);
	$q->set_table("t_form");
	$q->set_column(array("c_type","c_name","c_data"));
	$q->set_match("c_table",$table);
	$q->set_order("c_order");
	$q->run();
	$this->form	= $q->getall();
	$q->free();
      }
  };

?>
