<?
# $Header$
#
# $Log$
# Revision 1.4  2009-03-06 04:16:00  tino
# get_magic_quotes_gpc and hook_session
#
# Revision 1.3  2009-02-18 14:39:26  tino
# PHPSESSID now supported (oops)
#
# Revision 1.2  2009-02-01 23:28:58  tino
# Use of new SQL magics (and little bugfix)
#
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin
#

$lovinc	= dirname($_SERVER["SCRIPT_FILENAME"]);
# In lov_local.php do something like:
#include("sqlite.php");
#$db     = new DbSqlite("$lovinc/DB/db.sqlite");
include("$lovinc/lov_local.php");
# Now $db must be the database class.

# LOV FUNCTIONS

function lov_list2($mod,$proc,$nr,$var,$and="order by c_index")
{
  return db_q1all("select distinct c_index from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='?' $and", array($mod,$proc,$nr,$var));
}

function lov_index2($mod,$proc,$_nr,$var,$and="order by c_index")
{
  $list	= lov_list2($mod,$proc,$_nr,$var,$and);
  $idx	= array();
  foreach ($list as $v)
    $idx[$v]=1;
  return $idx;
}

function lov_list($mod,$proc,$nr,$var,$key,$and="order by c_index")
{
  db_begin();
  $list	= db_q1all("select c_index from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='?' and c_key='?' $and", array($mod, $proc, $nr, $var, $key));
  if (count($list)==0 && $key!='*')
    $list	= db_q1all("select c_index from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='?' and c_key='*' $and", array($mod, $proc, $nr, $var));
  if (count($list)==0 && $var!='*')
    $list	= db_q1all("select c_index from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='*' and c_key='?' $and", array($mod, $proc, $nr, $key));
  if (count($list)==0 && $key!='*' && $var!='*')
    $list	= db_q1all("select c_index from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='*' and c_key='*' $and", array($mod, $proc, $nr));
  db_end();
  return $list;
}

function lov_index($mod,$proc,$nr,$var,$key,$and="order by c_index")
{
  return array_flip(lov_list($mod,$proc,$nr,$var,$key,$and));
}

function lov_val_index($col,$mod,$proc,$nr,$var,$key,$index)
{
  for (;;)
    {
      $v	= db_q1("select $col from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='?' and c_key='?' and c_index='?' $and", array($mod, $proc, $nr, $var, $key, $index));
      if ($v!==false || $index=="*")
        return $v;
      $index	= "*";
    }
}
function lov_val_key($col,$mod,$proc,$nr,$var,$key,$index,$and)
{
  for (;;)
    {
      $v	= lov_val_index($col,$mod,$proc,$nr,$var,$key,$index,$and);
      if ($v!==false || $key=="*")
        return $v;
      $key	= "*";
    }
}
function lov_val_var($col,$mod,$proc,$nr,$var,$key,$index,$and)
{
  for (;;)
    {
      $v	= lov_val_key($col,$mod,$proc,$nr,$var,$key,$index,$and);
      if ($v!==false || $var=="*")
        return $v;
      $var	= "*";
    }
}

function lov_val($mod,$proc,$nr,$var,$key,$index,$and="")
{
  db_begin();
  $v	= lov_val_var("c_val",$mod,$proc,$nr,$var,$key,$index,$and);
  db_end();

  if ($v!==false)
    return $v;
  return "";
}

function lov_pw($pw,$sec="")
{
  if ($sec=="")
    $sec=lov_val("_LOV_","Settings",0,"Passwords","Scramble","*");
  if ($sec=="")
    $sec="LOV salt";
  return md5($sec.$pw.$sec);
}

function lov_id($mod,$proc,$nr,$var,$key,$index)
{
  return db_q1("select c_id from t_lov where c_mod='?' and c_proc='?' and c_nr=? and c_var='?' and c_key='?' and c_index='?'", array($mod, $proc, $nr, $var, $key, $index));
}

function lov_insert($mod,$proc, $nr, $var, $key, $index, $type, $val, $com="")
{
  db_begin();
  $i=db_inc("t_lov", "c_id");
  db_qok("insert into t_lov ( c_id, c_mod, c_proc, c_nr, c_var, c_key, c_index, c_type, c_val, c_comment, c_ts ) values ( ?, '?', '?', ?, '?', '?', '?', '?', '?', '?', ?NOW()? )", array( $i, $mod, $proc, $nr, $var, $key, $index, $type, $val, $com ));
  db_end();
}

function lov_update($id, $mod,$proc, $nr, $var, $key, $index, $type, $val, $com="")
{
  db_begin();
  $r=db_q0("update t_lov set c_mod='?', c_proc='?', c_nr=?, c_var='?', c_key='?', c_index='?', c_type='?', c_val='?', c_comment='?', c_ts=?NOW()? where c_id=?", array( $mod, $proc, $nr, $var, $key, $index, $type, $val, $com, $id ));
  db_end();
  return $r;
}

function lov_delete($id)
{
  return db_q0("delete from t_lov where c_id=?", array($id));
}

# ADDITIONAL HELPERS

$hook_session=null;
function cgi($vars)
{
  GLOBAL $hook_session;

  if (!is_array($vars))
    $vars	= explode(" ",$vars);
  $vars	= array_flip($vars);
  reset($_REQUEST);
  while (list($k,$v)=each($_REQUEST))
    {
      if (isset($vars[$k]))
        {
          $GLOBALS[$k]      = ( get_magic_quotes_gpc() ? stripslashes($_REQUEST[$k]) : $_REQUEST[$k] );
	  continue;
	}
      if ($k=="PHPSESSID")
        continue;
      die("lov.php: missing parameter $k");
    }
  header("Pragma: no-cache");
  header("Expires: 0");
  header("Cache-control: no-store,no-cache,max-age=0,must-revalidate");
  if ($hook_session)
    $hook_session();
}

# END

?>
