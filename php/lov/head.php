<?
# $Header$
#
# $Log$
# Revision 1.9  2010-09-19 19:48:52  tino
# extended shorten() functionality to h(), u() and hu()
# added class to a()
# changed the way head and menu are rendered:
# first ->head is called (before it was done after menu)
# then ->menu is created
# then ->body is called (replaces head)
# If menu present but body missing HR is printed (before: always)
# Do not use ->menuhook() anymore, use ->body()
#
# Revision 1.8  2009-07-24 10:59:43  tino
# Current
#
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

    var $head		= null;			# before menu()
    var $body		= null;			# function to call after menu() instead HR
    var $foot		= "";			# Content of the foot()er
    var $now		= "";			# Current time set with now()

    var $menu		= null;			# The menu structure, see menu.php

# Processing variables, do not touch:
    var $menu_in_bar	= 0;			# We are rendering the menu()
    var $form		= 0;			# Are we in a form
  };
$lov_head=new lov_head;

ob_start();

include("lov.php");
include("menu.php");
include("lister.php");
include("form.php");
include("mainmenu.php");

function shorten($s,$max,$ell='...')
{
  if ($max<1 || strlen($s)<=$max)
    return $s;
  return substr($s,0,$max<10+strlen($ell) ? $max : $max-strlen($ell)).$ell;
}
function u($s,$max=0,$ell='...')
{
  return rawurlencode(shorten($s,$max,$ell));
}

function h($s,$max=0,$ell='...')
{
  return htmlentities(shorten($s,$max,$ell));
}

function hu($s,$max=0,$ell='...')
{
  return h(u($s,$max,$ell));
}

function a($url,$txt,$class='')
{
  if (!$url)
    {
      echo h($txt);
      return;
    }
  echo '<a href="';
  echo h($url);
  echo '"';
  if ($class)
    echo " class=\"$class\"";
  echo '>';
  echo h($txt);
  echo '</a>';
}
function a1($u,$t,$c='')
{
  echo ' ';
  a($u,$t,$c);
}
function a2($u,$t,$c='')
{
  a($u,$t,$c);
  echo ' ';
}
function a3($u,$t,$c='')
{
  echo ' ';
  a($u,$t,$c);
  echo ' ';
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
  if ($lov_head->menuhook):
    die("Use ->body instead!");
  if ($lov_head->head)
    call_user_func($lov_head->head);
  if ($lov_head->menu)
    menu();
  if ($lov_head->body):
    call_user_func($lov_head->body);
  elseif ($lov_head->menu):
?><hr class="clear"/><?
  endif;
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
