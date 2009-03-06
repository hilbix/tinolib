<?
# $Header$
#
# $Log$
# Revision 1.2  2009-03-06 17:05:37  tino
# aif moved to head.php and bar_ functions added
#
# Revision 1.1  2009-02-03 17:13:14  tino
# Added
#

/*
$menu = array(
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

in_bar=0;
function bar_start()
{
  GLOBAL in_bar;

  in_bar	= 0;
}

function bar_add($a,$b,$c)
{
  GLOBAL in_bar;

  echo (in_bar ? " | " : "[ ");
  in_bar	= 1;
}

function bar_end()
{
  GLOBAL in_bar;

  if (in_bar)
    echo " ]\n";
  in_bar	= 0;
}

function menu($sect=0)
{
  GLOBAL $menu;

  bar_start();
  want	= 1;
  if ($sect>0)
    {
      echo "<br>";
      echo $menu[$sect];
      echo ": ";
      $want	= 0;
    }
  $me	= $_SERVER["SCRIPT_NAME"];
  $me2	= basename($me);
  $found= 0;
  $area	= 0;
  for (reset($menu); list($k,$v)=each($menu); )
    if (is_numeric($k))
      $area	= $k;
    else if ($k==$me || $k==$me2)
      $found	= $area;
  reset($menu);
  while (list($k,$v)=each($menu))
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

      bar_add($me!=$k && $me2!=$k, $k, $v);
    }
  bar_end();
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
