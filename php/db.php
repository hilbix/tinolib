<?
// $Header$
//
// This is part of tinolib.
// Copyright (C)2002-2004 by Valentin Hilbig
//
// Tinolib may be copied according to GNU GPL v2 or higher
// (Yes, GPL for now, not yet LGPL, sorry.)
//
// $Log$
// Revision 1.3  2004-06-03 05:13:17  tino
// database support starts to work
//
// Revision 1.2  2004/06/03 02:50:54  tino
// queryfull added
//
// Revision 1.1  2004/06/01 06:03:48  tino
// added.  This comes from an old version I often use

// This is far too mysql centric, I know
class tino_db
  {
    var $db, $debug;

    function tino_db($d="d_db", $u="u_user", $p="p_password")
      {
	$this->debug	= 0;
	$this->db	= @mysql_connect("localhost", $u, $p);
	if (!$this->db)
	  {
	    @readfile("sorry.txt");
	    die("sorry, cannot connect to database");
          }
	if (!mysql_select_db($d, $this->db))
	  die("missing database $d");
      }

    // beware, this is not perfect
    // If array $a is given, replace all =? occurences with strings
    // local function, not for public use:
    function q($s,$a)
      {
	if ($a!=null)
	  {
	    $e	= explode("=?",$s);
	    reset($e);
	    list($k,$v)=each($e);
	    $s	= $v;
	    while (list($k,$v)=each($e))
	      $s.="='".addslashes($a[$k-1])."'".$v;
	  }
	if ($this->debug)
	  echo "[query: $s]\n";
        return mysql_query($s,$this->db);
      }

    // Query exact 1 result:
    // select value from something where uniquecol=id
    function query1($s,$a=null)
      {
        $res = $this->q($s,$a);
	if (!$res)
	  return false;
        $arr	= mysql_fetch_row($res);
	if (mysql_fetch_row($res))
	  {
	    mysql_free_result($res);
	    return false;
          }
        mysql_free_result($res);
        return $arr[0];
      }

    // query with exact one line result
    // select * from something where uniquecol=id
    function query($s, $a=null)
      {
        $res = $this->q($s, $a);
	if (!$res)
	  return false;
        $arr	= mysql_fetch_array($res);
	if (mysql_fetch_row($res))
	  {
	    mysql_free_result($res);
	    return false;
          }
        mysql_free_result($res);
        return $arr;
      }

    // return full meshed array of resulting rows:
    // select A,B,C,D from something where multicol=val
    // returns array $arr with $arr[A][B][C]=D
    // probably only reasonable with "select distinct"
    // BEWARE, THAT'S SLOW!
    // SO ONLY ACCESS THIS WITH SMALL RESULT SETS!
    function queryfull($s, $a=null)
      {
        $res = $this->q($s, $a);
	if (!$res)
	  return false;
	$n	= mysql_num_fields($res)-1;
	if ($n<0)
	  return false;
        $arr	= array();
	while ($get=mysql_fetch_row($res))
	  {
	    if ($this->debug)
	      {
		echo "[fetch]\n";
		print_r($get);
	      }
	    // We must make heavy use of references here
	    $ref	=& $arr;
	    for ($i=0; $i<$n; $i++)
	      {
		// Already array?  If not, make it so.
		if (!is_array($ref))
		  $ref	= array();
		// Get reference to that sub-value
		$ref	=& $ref[$get[$i]];
	      }
	    // store the last value directly
	    $ref	= $get[$i];
	  }
        mysql_free_result($res);
        return $arr;
      }

    // return array of resulting rows
    // select COLS from something where multicol=val
    // returns array(row1,row2,row3,...)
    // where each row is an array
    function queryall($s, $a=null)
      {
        $res = $this->q($s, $a);
	if (!$res)
	  return false;
        $arr	= array();
	while ($get=mysql_fetch_array($res))
	  $arr[]	= $get;
        mysql_free_result($res);
        return $arr;
      }

    // return array of first value of rows
    // select col from something where multicol=val
    // returns array(row1.col, row2.col, row3.col, ...)
    function queryall1($s, $a=null)
      {
        $res = $this->q($s, $a);
	if (!$res)
	  return false;
        $arr	= array();
	while ($get=mysql_fetch_row($res))
	  $arr[]	= $get[0];
        mysql_free_result($res);
        return $arr;
      }

    // return associative array of value rows
    // select col1,col2 from something where multicol=val
    // returns array(row1.col1=>row1.col2, row2.col1=>row2.col2, ...)
    function queryall2($s,$a=null)
      {
        $res = $this->q($s,$a);
	if (!$res)
	  return false;
        $arr	= array();
	while ($get=mysql_fetch_row($res))
	  $arr[$get[0]]	= $get[1];
        mysql_free_result($res);
        return $arr;
      }

    // query with no results
    // update/replace/delete something
    function query0($s, $a=null)
      {
        $res=$this->q($s,$a);
	if (!$res)
		return false;
	$oops=@mysql_fetch_row($res);
	@mysql_free_result($res);
	return !$oops;
      }

    // this should give an object!
    function querystart($s,$a=null)
      {
        return $this->q($s,$a);
      }
    function querynext($res)
      {
        return mysql_fetch_row($res);
      }
    function querynext1($res)
      {
        $v = $this->querynext($res);
        if (!$v)
          return $v;
        return $v[0];
      }
    function queryend($res)
      {
        mysql_free_result($res);
      }

    function last_id()
      {
	return mysql_insert_id($this->db);
      }

    // Get field names
    // Works only once for now
    function get_fields($res)
      {
        $arr = array();
	for ($i=0; $i<mysql_num_fields($res); $i++)
          $arr[$i]	= mysql_fetch_field($res);
        return $arr;
      }
  };
?>
