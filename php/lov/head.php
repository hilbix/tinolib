<?
# $Header$
#
# $Log$
# Revision 1.2  2009-03-06 04:20:55  tino
# head_hook, delayed headers, functions u and hu, form close on foot
#
# Revision 1.1  2009-02-19 23:23:16  tino
# added

$form=0;

ob_start();

include("lov/lov.php");
include("lov/menu.php");
include("lov/lister.php");
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

function now()
{
  GLOBAL	$now;

  if (!isset($now))
    $now	= strftime("%Y-%m-%d %H-%M-%S");
  return $now;
}

$hook_head=null;
function head($name, $cgi=0, $init=0)
{
  GLOBAL $menu, $hook_head;

  $add	= ob_get_clean();
  cgi($cgi);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title><?=h($name)?></title>
<link rel="stylesheet" type="text/css" href="lov/css.php" />
<?=$add?>
</head>
<body<?if ($init):?> onload='init()'<?endif?>>
<?
  if ($menu):
  menu();
?><hr /><?
  endif;
  if ($hook_head)
    $hook_head();
}

function foot()
{
  GLOBAL $form, $foot;

  if ($form):
    form_close();
  endif;
  if ($foot):
?>
<hr />
<?=$foot?>
<?
  endif;
?>
</body>
</html>
<?
}
