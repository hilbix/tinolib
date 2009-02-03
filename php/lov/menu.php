<?
# $Header$
#
# $Log$
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

function aif($if, $url, $txt)
{
  if ($if)
    a($url, $txt);
  else
    echo h($txt);
}

function menu($sect=0)
{
  GLOBAL $menu;

  $c	= '[';
  if ($sect>0)
    {
      echo "<br>";
      echo $menu[$sect];
      echo ": ";
      $c	= "";
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
	    $c="[";
	  else
	    {
	      if (!$sect)
		break;
	      $c	= "";
	    }
	  continue;
        }
      else if ($c=="")
	continue;

      echo "$c ";
      aif($me!=$k && $me2!=$k, $k, $v);
      echo " ";
      $c	= '|';
    }
  echo "]\n";
  if ($sect<=0)
    {
      if ($found)
        {
#	  echo "| $v ]\n";
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
