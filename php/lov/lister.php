<?
# $Header$
#
# $Log$
# Revision 1.1  2009-02-19 23:22:05  tino
# Added
#

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
              aif($actions[$col], $actions[$col].h($row[$indexcol]), $a);
              ?> </div></td><?
            }
	  else
            {
              ?><td> <?
              aif($actions[$col], $actions[$col].h($row[$indexcol]), $a);
              ?> </td><?
            }
        }
      ?></tr><?
    }
  ?></table><?
}
?>
