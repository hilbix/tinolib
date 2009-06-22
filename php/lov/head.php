<?
# $Header$
#
# $Log$
# Revision 1.7  2009-06-22 20:34:39  tino
# Better head()
#
# Revision 1.6  2009-06-22 19:55:12  tino
# New global struct lov_head, needs rewrite in some bindings
#
# Revision 1.5  2009-05-07 00:09:37  tino
# CSS-file can be changed
#
# Revision 1.4  2009-03-06 17:29:48  tino
# aif() moved and include path corrected
#
# Revision 1.3  2009-03-06 17:05:49  tino
# aif() moved here
#
# Revision 1.2  2009-03-06 04:20:55  tino
# head_hook, delayed headers, functions u and hu, form close on foot

class lov_head
  {
    var $css		= "lov/css.php";	# CSS URL to use
    var $encoding	= null;			# What encoding to set in header

    var $head		= null;			# Additional function to call from head()
    var $foot		= "";			# Content of the foot()er
    var $now		= "";			# Current time set with now()

    var $menu		= null;
    var $menu_in_bar	= 0;

    var $form		= 0;			# Are we in a form
  };
$lov_head=new lov_head;

ob_start();

include("lov.php");
include("menu.php");
include("lister.php");
include("form.php");
include("mainmenu.php");

function u($s)
{
  return rawurlencode($s);   
}

function h($s)
{
  return htmlentities($s);   
}

function hu($s)
{
  return h(u($s));
}

function a($url,$txt)
{
  if (!$url)
    {
      echo h($txt);
      return;
    }
  echo '<a href="';
  echo h($url);
  echo '">';
  echo h($txt);
  echo '</a>';
}

function aif($if, $url, $txt)
{
  if ($if)
    a($url, $txt);
  else
    echo h($txt);
}

function stamp()
{
  GLOBAL $lov_head;

  if (!$lov_head->stamp)
    $lov_head->stamp	= time();
  return $lov_head->stamp;
}

function now()
{
  GLOBAL $lov_head;

  if ($lov_head->now=="")
    $lov_head->now	= strftime("%Y-%m-%d %H-%M-%S", stamp());
  return $lov_head->now;
}

function location($url)
{
  000;
}

function head($name, $cgi="", $init=0)
{
  GLOBAL $lov_head;

  $add	= ob_get_clean();
  cgi($cgi);
  if (is_array($name))
    $name	= implode("",$name);
  if ($lov_head->encoding)
    header("Content-type: text/html; charset=".$lov_head->encoding);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title><?=h($name)?></title>
<? if ($lov_head->encoding): ?>
<meta http-equiv="Content-type" content="text/html; charset=<?=$lov_head->encoding?>" />
<? endif ?>
<link rel="stylesheet" type="text/css" href="<?=$lov_head->css?>" />
<?=$add?>
</head>
<body<?if ($init):?> onload='init()'<?endif?>>
<?
  if ($lov_head->menu):
  menu();
?><hr /><?
  endif;
  if ($lov_head->head)
    call_user_func($lov_head->head);
}

function foot()
{
  GLOBAL $lov_head;

  if ($lov_head->form)
    form_close();
  if ($lov_head->foot)
    call_user_func($lov_head->foot);
?>
</body>
</html>
<?
}
