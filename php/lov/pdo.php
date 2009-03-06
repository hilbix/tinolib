<?php #-*-c-*-
# $Header$
#
# PDO wrapper
#
# Performance note:
# This does not use prepared statements,
# as my prepare differs a lot from the normal prepare, i. E:
#
# My variant:			Normal variant:		Difference:
# where col='?'			col=?			None
# where col=?			col=?			How do I state in the statement, that ? is numeric?
# create table a ( col ?INT? )	does not exist		How do I access driver independent features?
#
# Perhaps I must create a hybrid prepare statement, which transforms '?' to ? and assigns a string type,
# whereas a single ? then assigns a number, and ?NAME? sequence does the replace as usual.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.4  2009-03-06 16:57:02  tino
# For unknown reason _one() had 2 arguments, fixed that.
#
# Revision 1.3  2009-02-13 11:35:37  tino
# More PDO compliance
#
# Revision 1.2  2009-02-11 20:31:43  tino
# Bufixes due to last edit
#
# Revision 1.1  2009-02-11 14:50:10  tino
# SQLite3 now working via PDO
#

include("db.php");

class DbPDO extends Db
{
  var $driver, $lasterr;

  function DbPDO($name,$driver)
    {
      $this->type	= "PDO-$driver";
      $this->driver	= $driver;
      $this->lasterr	= "";
      parent::Db($name);
    }

#  function _init()
#    {
#      You MUST overwrite this to set $this->types.
#    }

  function _err()
    {
      return $this->lasterr;
    }

  function seterror($e)
    {
      $this->lasterr = $e[0]." ".$e[1].": ".$e[2];
    }

# This is just a quick hack
# Change all '?' sequences to ? sequences, but do not check the numeric property of the arg.
  function _assemble($q, $p, $a)
    {
      $s	= $p[0];
      if (is_array($a))
	{
	  if (count($p)!=count($a)+1)
	    $this->oops("statement needs ".(count($p)-1)." args: $q");
	  for ($i=1; $i<count($p); $i++)
	    if (substr($s,-1)=="'" && substr($p[$i],0,1)=="'")
	      $s	= substr($s,0,-1).'?'.substr($p[$i],1);
	    else if (!preg_match("/^[0-9][0-9]*$/", $a[$i-1]))
	      $this->oops("nonnumeric arg $i in query $q: '$v' at '$s'");
	    else
	      {
		$s	.= '?';
		$s	.= $p[$i];
	      }
	}
      else if (count($p)>1)
        $this->oops("statement needs args (but none given): $q");
      $this->debug($s);
      $p	= $this->db->prepare($s);
      if (!$p)
        {
          $this->seterror($this->db->errorInfo());
          return false;
        }
      return array($p,$a);
    }

  function PdoOpen($name, $user="", $pass="")
    {
      return new PDO($this->driver.':'.$name, $user, $pass);
    }

  function _escape($s)
    {
      return $this->db->quote($s);
    }

  function _c($r)
    {
      $r->closeCursor();
      $this->lastprep	= null;
    }

  function _execute($q)
    {
      if (is_array($q[1]))
        $r	= $q[0]->execute($q[1]);
      else
        $r	= $q[0]->execute();
      if ($r)
        return $q[0];
      $this->seterror($q[0]->errorInfo());
      $q[0]->closeCursor();
      return false;
    }

  function _query($q)
    {
      if ($q===false)
        return false;
      $q[0]->setFetchMode(PDO::FETCH_NUM);
      return $this->_execute($q);
    }

  function _query1($q)
    {
      if ($q===false)
        return false;
      $q[0]->setFetchMode(PDO::FETCH_COLUMN, 0);
      return $this->_execute($q);
    }
  function _one($r)
    {
      return $r->fetchAll(PDO::FETCH_COLUMN, 0);
    }
  function _single($r)
    {
      /* As it already has been prepared with query1
       * Hope this works as needed here (PHP's documentation is a mess)!
       */
      return $r->fetch(PDO::FETCH_COLUMN, 0);
    }

  function _row($r)
    {
      return $r->fetch(PDO::FETCH_NUM);
    }
  function _all($r)
    {
      return $r->fetchAll(PDO::FETCH_NUM);
    }

  function begin()
    {
      if (!$this->db)
        $this->_start();
      $this->db->beginTransaction();
    }
  function rollback()
    {
      $this->db->rollback();
    }
  function end()
    {
      $this->db->commit();
    }
};

?>
