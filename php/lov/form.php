<?
$form=0;

function form_close()
{
  GLOBAL $form;

  if ($form):
    form_open();
    $form	= 0;
?></form><?
  endif;
}

function form_open($name="", $url='')
{
  GLOBAL $form;

  if ($form==2):
    $form=1;
?></select><?
  endif;
  if ($form)
    return;
  $form	= 1;
?><form method="post"<?if ($name):?> name="<?=h($name)?>"<?endif; if ($url):?> action="<?=h($url)?>"<?endif?>><?
}

function hidden($tag, $val)
{
  form_open();
?><input type="hidden" name="<?=h($tag)?>" value="<?=h($val)?>" /><?
}

function input($tag, $val, $width=20)
{
  form_open();
?><input type="text" name="<?=h($tag)?>" value="<?=h($val)?>" size="<?=h($width)?>" /><?
}

function password($tag, $val, $width=20)
{
  form_open();
?><input type="password" name="<?=h($tag)?>" value="<?=h($val)?>" size="<?=h($width)?>" /><?
}

function textarea($tag, $val, $width=80, $height=10)
{
  form_open();
?><textarea name="<?=h($tag)?>" cols="<?=h($width)?>" rows="<?=h($height)?>"><?=h($val)?></textarea><?
}

function submit($tag, $val)
{
  form_open();
?><input type="submit" name="<?=h($tag)?>" value="<?=h($val)?>" /><?
}

function checkbox($tag, $val=1, $checked=0)
{
  form_open();
?><input type="checkbox" name="<?=h($tag)?>" value="<?=h($val)?>" <? if ($checked):?>checked="checked"<?endif?> /><?
}

function radio($tag, $val=1, $checked=0)
{
  form_open();
?><input type="radio" name="<?=h($tag)?>" value="<?=h($val)?>" <? if ($checked):?>checked="checked"?><?endif?> /><?
}

function select($tag)
{
  GLOBAL $form;

  form_open();
  $form=2;
?><select name="<?=h($tag)?>"><?
}

function options($opt, $val)
{
  if (!is_array($opt))
    $opt	= array($opt=>$opt);
  foreach ($opt as $k=>$v):
?><option value="<?=h($k)?>"><?=h($v)?></option><?
  endforeach;
}
?>
