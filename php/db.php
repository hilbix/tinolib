<?
// $Header$
//
// $Log$
// Revision 1.1  2004-06-01 06:03:48  tino
// added.  This comes from an old version I often use
//

// This is far too mysql centric, I know
class tino_db
  {
    var $db;

    function tino_db($d="d_db", $u="u_user", $p="p_password")
      {
	$this->db = @mysql_connect("localhost", $u, $p);
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
#print $s;
	  }
        return mysql_query($s,$this->db);
      }

    // Query exact 1 result:
    // select value from something where key=id
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
    // select * from something where key=id
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

    // return array of resulting rows
    // select * from something where multicol=val
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
    // select col from something where multicol=val
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
    function get_fields($res)
      {
        $arr = array();
	for ($i=0; $i<mysql_num_fields($res); $i++)
          $arr[$i]	= mysql_fetch_field($res);
        return $arr;
      }
  };
?>
