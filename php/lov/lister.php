<?
# $Header$
#
# $Log$
# Revision 1.6  2009-03-08 11:34:11  tino
# Added additional parameter to print in horizontal menu
#
# Revision 1.5  2009-03-06 17:32:02  tino
# lister moved to new menu_ functions
#
# Revision 1.3  2009-03-06 04:16:47  tino
# Url-Escapes
#
# Revision 1.2  2009-02-28 17:05:19  tino
# listhor

function listhor($q, $i, $t, $l, $k=-1, $a="",$b="")
{
  GLOBAL $host;

  menu_start();
  foreach ($q as $v)
    {
      $kk="";
      if ($k>=0)
        $kk	= "$a$v[$k]$b";
      if ($v[$t]=="")
        menu_add($l.$v[$i], "empty", $host!=$v[$i], "(", ")$kk");
      else
        menu_add($l.$v[$i], $v[$t], $host!=$v[$i], "", $kk);
    }
  menu_end();
}

function lister($rows, $headings, $indexcol, $actions)
{
  ?><table><tr><?
  foreach ($headings as $v)
    {
      ?><th> <?=h($v)?> </th><?
    }
  ?></tr><?
  $i	= 0;
  foreach ($rows as $row)
    {
      $i++;
      ?><tr class="line<?=$i&1?>"><?
      foreach ($headings as $col=>$ign)
        {
          if (is_array($actions[$col]))
            {
              $a	= $actions[$col];
              ?><td> <?
	      foreach ($actions[$col] as $tag=>$type):
		echo "$type";
	      endforeach;
              ?> </td><?
            }
          $a	= is_numeric($col) ? $row[$col] : $col;
	  if (strlen($a)>50)
	    {
              ?><td><div class="m500"> <?
              aif($actions[$col], $actions[$col].hu($row[$indexcol]), $a);
              ?> </div></td><?
            }
	  else
            {
              ?><td> <?
              aif($actions[$col], $actions[$col].hu($row[$indexcol]), $a);
              ?> </td><?
            }
        }
      ?></tr><?
    }
  ?></table><?
}
?>
