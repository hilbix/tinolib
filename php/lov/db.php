<? #-*-c-*-
# $Header$
#
# Generic database wrapper.
# This shall be XHTML 1.0 compliant.
#
# The database has a "slow start",
# such that you still can access pages which do not use the database
# if the database is down.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.3  2009-02-11 14:50:09  tino
# SQLite3 now working via PDO
#
# Revision 1.2  2009-02-01 21:06:38  tino
# MySQL support added
#
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin

$db=null;

class Db
{
  var	$db, $name, $type, $debugging = 0;

  /* Following types must be set in the _init() function,
   * they are replace values of ?NAME? within statements:
   *
   * Datatypes:
   * INT	maximum integer type
   * FLOAT	maximum numeric type (floating point)
   * VARCHAR	datatype to keep 255 characters minimum (indexable)
   * TEXT	text blob type (non indexable, min 2GB size)
   * BLOB	data blob type (non-indexable, min 2GB size)
   * TIMESTAMP	timestamp datatype (this is for UTC timestamps)
   * DATETIME	datetime datatype (this handles date and time)
   *
   * Functions:
   * NOW()	function to return current localtime for DATETIME
   * TS()	function to return current GMT timestamp for TIMESTAMP
   */
  var	$types;

  function _err()
    {
      return "(unknown error)";
    }

  function lasterr()
    {
      return $this->_err();
    }

  function _start()
    {
      $this->db	= $this->_open($this->name);
      if (!$this->db)
	$this->oops("cannot open ".$this->type." DB ".$this->name.": ".$this->lasterr());
      $this->_init();
    }

  function Db($name)
    {
      $this->name=$name;
    }

  function oops($s)
    {
      die("OOPS ".htmlentities($s).": ".$this->lasterr());
    }
  function debug_escape($s)
    {
      echo str_replace(array("--","\$","\""),array("\\-\\-","\\\$", "\\\""), str_replace("\\","\\\\",$s));
    }
  function debug()
    {
      if (!$this->debugging)
        return;
      echo $this->debugging==1 ? "<!-- " : "<pre>debug ";
      echo $this->name;
      echo ": ";
      $a	= func_get_args();
      if (count($a)==1)
	$a	= $a[0];
      if (is_string($a) && substr($a,0,6)=="select")
	{
	  echo "\"";
	  $this->debug_escape($a);
	  echo "\"";
	}
      else
        {
          $this->debug_escape(str_replace("   ", "", str_replace("\n"," ",print_r($a, true))));
        }
      echo $this->debugging==1 ? " -->\n" : "</pre>\n";
    }

  # Assemble the statement
  # The default is to return the string to execute
  # But this can also prepare() a statement
  function _assemble($q,$p,$a)
    {
      $s	= $p[0];
      if (is_array($a))
	{
	  if (count($p)!=count($a)+1)
	    $this->oops("statement needs ".(count($p)-1)." args: $q");
	  for ($i=1; $i<count($p); $i++)
	    {
	      $v	= $a[$i-1];
	      if (substr($s,-1)=="'" && substr($p[$i],0,1)=="'")
		$v	= $this->_escape($v);
	      else if (!preg_match("/^[0-9][0-9]*$/", $v))
		$this->oops("nonnumeric arg $i in query $q: $v");
	      $s	.= $v;
	      $s	.= $p[$i];
	    }
	}
      else if (count($p)>1)
        $this->oops("statement needs args (but none given): $q");
      $this->debug($s);
      return $s;
    }

  function _prep($q,$a)
    {
      if (!preg_match("!^[A-Za-z0-9][-.,='_A-Za-z0-9 +*/?()]*\$!", $q))
	$this->oops("unknown character in query: $q");

      $p	= explode("?", $q);

      # replace ?TYPE? with some type definitions
      for ($i=count($p)-1; --$i>=1; )
        if (isset($this->types[$p[$i]]))
          {
            /* we have 3 elements START ?MAGIC? TAIL, this becomes one element STARTreplacementTAIL */
            $p[$i-1].=$this->types[$p[$i]].$p[$i+1];	/* append it to the previous string and join the following, too	*/
            array_splice($p, $i, 2);			/* remove the two joined elements	*/
	    $i--;
          }
      return $this->_assemble($q,$p,$a);
    }

  # Workaround for missing "fetchAll" type
  function _rowarray($r,$f)
    {
      $a	= array();
      while (($d=$this->$f($r))!==false)
	$a[]	= $d;
      return $a;
    }
  # Workaround for missing "fetchAll of single column" type
  function _one($r)
    {
      return $this->_rowarray($r, "_single");
    }
  function _all($r)
    {
      return $this->_rowarray($r, "_row");
    }
  # Default for missing query returing single row
  function _query1($s)
    {
      return $this->_query($s);
    }

  # Run Query
  # Query is supposed to return following:
  # false			on errors
  # true or something not 0	on success
  # If the query results in data, there must be some rowset.
  # If the query results in no data but is OK, there can be an
  # (empty) which is not 0
  function _q($s,$a=0)
    {
      if (!$this->db)
        $this->_start();
      $this->lastprep	= $this->_prep($s,$a);
      $r	= $this->_query($this->lastprep);
      $this->debug($r);
      return $r;
    }
  # Single row preparation
  function _q1($s,$a=0)
    {
      if (!$this->db)
        $this->_start();
      $this->lastprep	= $this->_prep($s,$a);
      $r	= $this->_query1($this->lastprep);
      $this->debug($r);
      return $r;
    }
  function _qq($r)
    {
      if (!$r)
	$this->oops("no rows for ".$this->lastprep);
      return $r;
    }

  # Close Query
  function _c($r)
    {
      $this->lastprep	= null;
    }

  # Default _single() call
  function _single($r)
    {
      $a	= $this->_row($r);
      return $a ? $a[0] : $a;
    }

  # Run a query without result
  function q0($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if ($r===false)
	return false;
      $this->_c($r);
      return true;
    }
  function qok($q,$a=0)
    {
      $r	= $this->_qq($this->q($q,$a));
      $this->_c($r);
    }

  # Return exactly one singleton argument
  function q1($q,$a=0)
    {
      $r	= $this->_q1($q,$a);
      if ($r===false)
	return "";
      $v	= $this->_single($r);
      $this->_c($r);
      $this->debug("q1", $v);
      return $v;
    }
  # for all rows
  function q1all($q,$a=0)
    {
      $r	= $this->_q1($q,$a);
      if ($r===false)
	return false;
      $a	= $this->_one($r);
      $this->_c($r);
      $this->debug("q1all", $a);
      return $a;
    }
  # value must exist
  function q1arr($q,$a=0)
    {
      $r	= $this->_qq($this->q1($q,$a));
      $a	= $this->_one($r);
      $this->_c($r);
      $this->debug("q1arr", $a);
      return $a;
    }
  # One complete row as array
  function qrow($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if ($r===false)
	return false;
      $a	= $this->_row($r);
      $this->_c($r);
      $this->debug("qrow", $a);
      return $a;
    }
  # All rows as array of arrays
  function qall($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if ($r===false)
	return false;
      $a	= $this->_all($r);
      $this->_c($r);
      $this->debug("qall", $a);
      return $a;
    }
  # value must exist
  function qarr($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if ($r===false)
	$this->oops("no rows for $q");
      $a	= $this->_all($r);
      $this->_c($r);
      $this->debug("qarr", $a);
      return $a;
    }

  # Query assoc

  # tuple as array
  function q2all($q,$a=0)
    {
      $a	= $this->qall($q,$a);
      if (!$a)
	return false;
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=$v[1];
      $this->debug("q2all", $b);
      return $b;
    }
  # value must exist
  function q2arr($q,$a=0)
    {
      $a	= $this->qarr($q,$a);
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=$v[1];
      $this->debug("q2arr", $b);
      return $b;
    }

  # vector with first element is the array key
  function qxall($q,$a=0)
    {
      $a	= $this->qall($q,$a);
      if (!$a)
	return false;
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=array_slice($a,1);
      $this->debug("qxall", $b);
      return $b;
    }
  # value must exist
  function qxarr($q,$a=0)
    {
      $a	= $this->qarr($q,$a);
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=array_slice($a,1);
      $this->debug("qxarr", $b);
      return $b;
    }

  # transactional
  function begin()
    {
      $this->qok("begin");
    }
  function rollback()
    {
      $this->qok("rollback");
    }
  function end()
    {
      $this->qok("commit");
    }

  # next increment of a column
  function inc($table,$column)
    {
      return $this->q1("select 1+ifnull(max($column),0) from $table");
    }
};

# Convenience wrappers

function db_q0($s,$a=0)
{
  GLOBAL $db;

  return $db->q0($s,$a);
}

function db_qok($s,$a=0)
{
  GLOBAL $db;

  $db->qok($s,$a);
}

function db_q1($s,$a=0)
{
  GLOBAL $db;

  return $db->q1($s,$a);
}

function db_q1all($s,$a=0)
{
  GLOBAL $db;

  return $db->q1all($s,$a);
}

function db_q1arr($s,$a=0)
{
  GLOBAL $db;

  return $db->q1arr($s,$a);
}

function db_qrow($s,$a=0)
{
  GLOBAL $db;

  return $db->qrow($s,$a);
}

function db_qall($s,$a=0)
{
  GLOBAL $db;

  return $db->qall($s,$a);
}

function db_qarr($s,$a=0)
{
  GLOBAL $db;

  return $db->qarr($s,$a);
}

function db_q2all($s,$a=0)
{
  GLOBAL $db;

  return $db->q2all($s,$a);
}

function db_q2arr($s,$a=0)
{
  GLOBAL $db;

  return $db->q2arr($s,$a);
}

function db_qxall($s,$a=0)
{
  GLOBAL $db;

  return $db->qxall($s,$a);
}

function db_qxarr($s,$a=0)
{
  GLOBAL $db;

  return $db->qxarr($s,$a);
}




function db_($s,$a=0)
{
  GLOBAL $db;

  return $db->_($s,$a);
}




function db_begin()
{
  GLOBAL $db;

  $db->begin();
}

function db_rollback()
{
  GLOBAL $db;

  $db->rollback();
}

function db_end()
{
  GLOBAL $db;

  $db->end();
}

function db_inc($t,$c)
{
  GLOBAL $db;

  return $db->inc($t, $c);
}

?>
