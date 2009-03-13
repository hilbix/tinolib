<? #-*-c-*-
# $Header$
#
# $sel	= new db_select("table");
# $sel->col("col1,col2,?", $arg)->order("col1")->limit(100);
# if ($whatever) $sel->where("col2=?", $whatever);
# $r	= db_qall($sel->sql(), $sel->args);
#
# With joins:
#
# $sel	= new db_select("table a");
# $sel->col("col1,col2,?", $arg)->order("col1")->limit(100);
# $sel->join("jointable b", "left")->where("b.rowid=a.col")->where("b.col='?'",$value);
# etc.
#
# $Log$
# Revision 1.1  2009-03-13 08:02:09  tino
# added
#

class db_select
  {
    var $args, $joins;
    var $tables, $cols, $wheres, $groups, $orders;
    var $whereargs, $colargs;
    var $limit_from, $limit_max;

    function db_select($tables)
      {
        $this->args		= null;
        $this->joins		= array();
        $this->tables		= $tables;
	$this->cols		= "";
	$this->colargs		= array();
	$this->wheres		= "";
	$this->whereargs	= array();
	$this->groups		= "";
	$this->orders		= "";
        $this->limit_from	= false;
        $this->limit_max	= false;
      }
    function _arg(&$arr, $arg)
      {
        if ($arg===null)
          return;
	if (is_array($arg))
	  $arr	= array_merge($arr, $arg);
	else
          $arr[]	= $arg;
      }
    function sql()
      {
        $s		= "select ".substr($this->cols,1)." from ".$this->tables;
        $this->args	= $this->colargs;
	foreach ($this->joins as $j)
	  {
            list($a,$b)=$j;
	    $s		.= " $a join ".$b->tables." on ".substr($b->wheres,5);
	    $this->args	=  array_merge($this->args, $b->whereargs);
	  }
	if ($this->wheres!=="")
	  {
	    $s		.= " where ".substr($this->wheres,5);
	    $this->args	=  array_merge($this->args, $this->whereargs);
          }
	if ($this->groups!=="")
	  $s	.= " group by ".substr($this->groups,1);
	if ($this->orders!=="")
	  $s	.= " order by ".substr($this->orders,1);
        if ($this->limit_max!==false)
          {
	    if ($this->limit_from===false)
	      $s	.= " limit ".$this->limit_max;
            else
	      $s	.= " limit ".$this->limit_from.",".$this->limit_max;
          }
#        echo "SQL=$s<br>";
        return $s;
      }

    function where($and, $arg=null)
      {
        $this->wheres	.= " and $and";
	$this->_arg($this->whereargs, $arg);
        return $this;
      }
    function col($col, $arg=null)
      {
        $this->cols	.= ",$col";
        $this->_arg($this->colargs, $arg);
        return $this;
      }
    function order($ord)
      {
	$this->orders	.= ",$ord";
        return $this;
      }
    function group($group)
      {
	$this->groups	.= ",$group";
        return $this;
      }
    function from($lim)
      {
        $this->limit_from	= $lim;
        return $this;
      }
    function limit($lim)
      {
        $this->limit_max	= $lim;
        return $this;
      }
    function join($tables, $type="")
      {
        $j		= new db_select($tables);
	$this->joins[]	= array($type, $j);
        return $j;
      }
  }
?>
