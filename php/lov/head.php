<?
# $Header$
#
# $Log$
# Revision 1.1  2009-02-19 23:23:16  tino
# added
#

ob_start();

include("lov/lov.php");
include("lov/menu.php");
include("lov/lister.php");
include("mainmenu.php");

function h($s)
{
  return htmlentities($s);   
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

function head($name, $cgi, $init=0)
{
  GLOBAL $menu;

  cgi($cgi);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title><?=h($name)?></title>
<link rel="stylesheet" type="text/css" href="lov/css.php" />
<? ob_end_flush() ?>
</head>
<body<?if ($init):?> onload='init()'<?endif?>>
<?
  if ($menu):
  menu();
?><hr /><?
  endif;
}

function foot()
{
?>
</body>
</html>
<?
}
?>
