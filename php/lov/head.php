<?
# $Header$
#
# $Log$
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

$form=0;
$css="lov/css.php";

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
  GLOBAL $menu, $hook_head, $css;

  $add	= ob_get_clean();
  cgi($cgi);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title><?=h($name)?></title>
<link rel="stylesheet" type="text/css" href="<?=$css?>" />
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
