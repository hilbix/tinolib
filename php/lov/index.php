<?
# $Header$
#
# $Log$
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin
#

include("lov.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LOV edit</title>
<link rel="stylesheet" type="text/css" href="css.php" />
</head>
<body>
<?
if (!@lov_id("_LOV_","Settings",0,"Variables","*","Variables"))
  {
    @db_q0('create table t_lov ( c_id number not null,c_mod not null,c_proc not null,c_nr number not null,c_var not null,c_key not null,c_index not null,c_type not null,c_val, c_comment,c_ts,primary key (c_id));');
    @db_q0('create unique index i_lov on t_lov ( c_mod,c_proc,c_nr,c_var,c_key,c_index );');

    lov_insert("_LOV_","Settings",0,"Variables","*","Modules","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Variables","*","Passwords","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Variables","*","Processes","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Variables","*","Types","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Variables","*","Variables","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Processes","_LOV_","Settings","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Types","Types","-","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Types","Variables","-","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Types","Passwords","hash","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Types","Passwords","str","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Types","*","str","-","","Automatically created");
    lov_insert("_LOV_","Settings",0,"Modules","*","_LOV_","-","","Automatically created");
    ob_start();
    echo time();
    echo date();
    print_r(gettimeofday());
    phpinfo();
    lov_insert("_LOV_","Settings",0,"Passwords","Scramble","*","str",md5(ob_get_clean()),"Automatically created");
  }

function h($s)
{
  return htmlentities($s);
}

function pw_check($_pw)
{
  $passdef=lov_val("_LOV_","Settings",0,"Passwords","Default",$_SERVER["SCRIPT_NAME"]);
  $passmd5="";
  @include("lov_pw.php");

  if ($passmd5=="" && $passdef!="")
    {
      $pw	= lov_pw($_pw);
      $passmd5	= $passdef;
    }
  else
    {
      $pw	= lov_pw($_pw, "LOV init");
      if ($passdef!="")
        echo "<p><b>Warning!</b> Initial password still active.  To deactivate remove lov_pw.php.</p>";
      else if ($passmd5!="")
        echo "<p><b>Hint:</b> Create your password under '_LOV_', 'Settings', 0, 'Passwords', 'Default', '*'</p>";
    }
  if ($pw!=$passmd5)
      {
        if ($passmd5=="" && $_pw!=""):
?>
<h2>Password not set!</h2>
<p>
To set the given text as the initial password, create file lov_pw.php with following contents:
</p>
<table summary="contents of file lov_pw.php"><tr><td>&lt;?php $passmd5="<?=$pw?>"; ?&gt;</td></tr></table>
<p>
Then reload this page.
On Unix you can do:
</p>
<table summary="unix line to create lov_pw.php"><tr><td>echo '&lt;?php $passmd5="<?=$pw?>"; ?&gt;' &gt;lov_pw.php</td></tr></table>
<p>
</p>
<?
	endif;
	die("If you have password trouble, create file lov_pw.php with contents: &lt;?php \$passdef=''; ?&gt;");
      }
}

function getval($v)
{
  GLOBAL $r_mo, $r_pr, $r_nr, $r_vr, $r_ky, $r_in, $r_ty, $r_va;

  $r_mo	= trim($_POST["mod$v"]);
  $r_pr	= trim($_POST["proc$v"]);
  $r_nr	= trim($_POST["nr$v"]);
  $r_vr	= trim($_POST["var$v"]);
  $r_ky	= trim($_POST["key$v"]);
  $r_in	= trim($_POST["ind$v"]);
  $r_ty	= trim($_POST["typ$v"]);
  $r_va	= trim($_POST["val$v"]);

  if ($r_vr=="Passwords" && $r_ty!="hash")
    {
      $r_ty	= "hash";
      $r_va	= lov_pw($r_va);
    }
}

$set="";
if (isset($_POST["set"]))
  {
    $set	= $_POST["set"];
    pw_check($set);
    getval("N");
    if ($r_mo!="" && $r_pr!="" && $r_nr!="" && $r_vr!="" && $r_ky!="" && $r_in!="" && $r_ty!="")
      lov_insert($r_mo, $r_pr, $r_nr, $r_vr, $r_ky, $r_in, $r_ty, $r_va);
    if (isset($_POST["del"]) && isset($_POST["delete"]) && $_POST["del"]!="" && $_POST["delete"]!="")
      {
        foreach ($_POST["sel"] as $v)
          if (strval(intval($v))==$v && $v)
            lov_delete($v);
      }
    if (isset($_POST["save"]) && $_POST["save"]!="")
      {
        foreach ($_POST["sel"] as $v)
          {
            getval($v);
	    if ($r_mo!="" && $r_pr!="" && $r_nr!="" && $r_vr!="" && $r_ky!="" && $r_in!="" && $r_ty!="")
              lov_update($v, $r_mo, $r_pr, $r_vr, $r_ky, $r_in, $r_ty, $r_va);
          }
      }
    if (isset($_POST["add"]) && $_POST["add"]!="")
      {
        foreach ($_POST["sel"] as $v)
          {
            getval($v);
	    if ($r_mo!="" && $r_pr!="" && $r_nr!="" && $r_vr!="" && $r_ky!="" && $r_in!="" && $r_ty!="")
              lov_insert($r_mo, $r_pr, $r_nr, $r_vr, $r_ky, $r_in, $r_ty, $r_va);
          }
      }
  }

function sel_it($arr,$def,$_var)
{
  if ($_var=="")
    $_var=$def;
  $var	= h($_var);
  if (!isset($arr[$var]))
    echo "<option value='$var' selected>* $var * (unknown / do not use)</option>";
  foreach ($arr as $k=>$v)
    {
      if ($k==$_var)
        echo "<option selected>";
      else
        echo "<option>";
      echo h($k);
      echo "</option>";
    }
}

######################################################################################
$processes_var="";
$types_var="";
######################################################################################
function modules($var)
{
  GLOBAL $modules, $processes_var;

  $processes_var	= $var;
  sel_it($modules, "_LOV_",$var);
}

function variables($var)
{
  GLOBAL $variables, $types_var;

  $types_var	= $var;
  sel_it($variables, "Variables",$var);
}

function processes($s)
{
  GLOBAL $processes, $processes_var;

  $a	= $processes[$processes_var];
  if (!is_array($a))
    {
      if ($processes_var=="")
        $a	= lov_index2('_LOV_', 'Settings', 0, 'Processes');
      else
	$a	= lov_index('_LOV_', 'Settings', 0, 'Processes', $processes_var);
      $types[$processes_var]=$a;
    }
  sel_it($a,"Settings",$s);
}

function types($s)
{
  GLOBAL $types, $types_var;

  $a	= $types[$types_var];
  if (!is_array($a))
    {
      if ($types_var=="")
        $a	= lov_index2('_LOV_', 'Settings', 0, 'Types');
      else
	$a	= lov_index('_LOV_', 'Settings', 0, 'Types', $types_var);
      $types[$types_var]=$a;
    }
  sel_it($a,"str",$s);
}

$modules=lov_index('_LOV_', 'Settings', 0, 'Modules', '*');
$variables=lov_index('_LOV_', 'Settings', 0, 'Variables', '*');
$types=array();
$processes=array();

function field($n,$v,$c,$t)
{
  if (intval($t).""=="$t")
    {
      if (intval($t)>0)
        {
	  $v	= h($v);
          echo "<input type=text size=$t name='$c$n' value='$v' />";
	}
      return;
    }
  echo "<select name='$c$n'>";
  $t($v);
  echo "</select>";
}
?>
<form method=post action="#j">
<?if ($set!=""):?>
<table summary="list of values"><tr><th></th>
<th>Module</th>
<th>Process</th>
<th>Nr</th>
<th>Variable</th>
<th>Key</th>
<th>Index</th>
<th>Type</th>
<th>Value</th>
<th>Comment</th>
<th>Timestamp</th>
</tr>
<?
  $col	= array("id"=>-1,"mod"=>"modules","proc"=>"processes","nr"=>3,"var"=>"variables","key"=>10,"ind"=>10,"typ"=>"types","val"=>20,"com"=>20,"ts"=>0);
  $list	= db_qall("select c_id,c_mod,c_proc,c_nr,c_var,c_key,c_index,c_type,c_val,c_comment,c_ts from t_lov order by c_var DESC,c_key,c_index");
  $line = 0;
  $hadedit=0;
  foreach ( $list as $v ):
    $line++;
    $n	= 0;
    $id	= h(0+$v[0]);
?>
<tr class="line<?=$line&1?>">
<? if (isset($_POST["e$id"])): ?>
<? if (!$hadedit): ?>
<a name="j"></a>
<? endif ?>
<? $hadedit=1 ?>
<td><input type=checkbox name="sel[]" value="<?=$id?>" checked /></td>
<? foreach ($col as $c=>$w): ?>
<? if ($w!==-1): ?>
<td><?field($id,$v[++$n],$c,$w)?></td>
<? endif ?>
<? endforeach ?>
<? else: ?>
<td><input type=checkbox name="e<?=$id?>" value="x" /></td>
<? foreach ($col as $c=>$w): ?>
<? if ($w!==-1): ?>
<td><?=h($v[++$n])?></td>
<? endif ?>
<? endforeach ?>
<? endif ?>
</tr>
<? endforeach; ?>
<tr class="line<?=($line+1)&1?>">
<? if ($hadedit): ?>
<td><input type=checkbox name="del" /></td><td colspan=5>confirm <input type=submit name=delete value="delete" />
or <input type=submit name=save value="save" />
or <input type=submit name=add value="add" />
</td>
<? else: ?>
<td><a name="j">+</a></td>
<? foreach ($col as $c=>$w): ?>
<? if ($w!==-1): ?>
<td><?field("N",$_POST[$c."N"],$c,$w)?></td>
<? endif ?>
<? endforeach ?>
<? endif ?>
</tr>
</table>
<?endif;?>
Setup: <input type=text size=10 name=set value="<?=htmlentities($set)?>" /><input type=submit value="Edit" />
</form>
</body>
</html>
