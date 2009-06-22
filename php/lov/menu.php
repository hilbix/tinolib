<?
# $Header$
#
# $Log$
# Revision 1.6  2009-06-22 20:40:12  tino
# LF
#
# Revision 1.5  2009-06-22 19:53:27  tino
# Menu moved into global struct lov_head
#
# Revision 1.4  2009-03-07 07:35:23  tino
# Empty text now is called "empty"
#
# Revision 1.3  2009-03-06 17:29:14  tino
# bar_ functions renamed menu_ and menu_add() now knows prefix and suffix
#
# Revision 1.2  2009-03-06 17:05:37  tino
# aif moved to head.php and bar_ functions added

/*
$lov_head->menu = array(
	'index.php'		=> 'Home'
,	'status.php'		=> 'Status'
,	'graph.php'		=> 'Graph'
,	'links.php'		=> 'Links'
,	'menu.php'		=> 'More..'
,	1			=> 'Information'
,	'legals.php'		=> 'Legals'
,	'news.php'		=> 'News&amp;Bugs'
,	'faq.php'		=> 'FAQ'
,	'bittorrent.php'	=> 'BitTorrent'
,	'mirror.php'		=> 'Mirror'
,	2			=> 'Administrative'
,	'b32.php'		=> 'Unsupported'
,	'blocked.php'		=> 'Blocked'
,	'downtime.php'		=> 'Downtime'
,	'closedown.php'		=> 'Closedown'
,	'disable.php'		=> 'Disabled'
);
*/

function menu_start()
{
  GLOBAL $lov_head;

  $lov_head->menu_in_bar	= 0;
}

function menu_add($link,$text, $showlink=1, $pref="", $suff="")
{
  GLOBAL $lov_head;

  echo ($lov_head->menu_in_bar ? "\n| " : "[ ");
  if ($text=="")
    {
      $text	= "empty";
      if ($pref=="")
        $pref	= "(";
      if ($suff=="")
        $suff	= ")";
    }
  echo $pref;
  aif($showlink,$link,$text);
  echo $suff;
  $lov_head->menu_in_bar	= 1;
}

function menu_end()
{
  GLOBAL $lov_head;

  if ($lov_head->menu_in_bar)
    echo "\n]\n";
  $lov_head->menu_in_bar	= 0;
}

function menu($sect=0)
{
  GLOBAL $lov_head;

  menu_start();
  $want	= 1;
  if ($sect>0)
    {
      echo "<br>";
      echo $lov_head->menu[$sect];
      echo ": ";
      $want	= 0;
    }
  $me	= $_SERVER["SCRIPT_NAME"];
  $me2	= basename($me);
  $me3	= $me2.".php";
  $found= 0;
  $area	= 0;
  for (reset($lov_head->menu); list($k,$v)=each($lov_head->menu); )
    if (is_numeric($k))
      $area	= $k;
    else if ($k==$me || $k==$me2 || $k==$me3)
      $found	= $area;
  reset($lov_head->menu);
  while (list($k,$v)=each($lov_head->menu))
    {
      if (is_numeric($k))
        {
          if ($sect==$k)
	    $want	= 1;
	  else
	    {
	      if (!$sect)
		break;
	      $want	= 0;
	    }
	  continue;
        }
      else if (!$want)
	continue;

      menu_add($k, $v, $me!=$k && $me2!=$k && $me3!=$k);
    }
  menu_end();
  if ($sect<=0)
    {
      if ($found)
        {
          menu($found);
	  return;
        }
      else if ($sect<0)
        {
          for ($found=1; $found<=$area; $found++)
            menu($found);
          return;
        }
    }
}
?>
