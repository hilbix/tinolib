<?

function form_start($name, $url='')
{
?><form method="post" name="<?=h($name)?>"<?if ($url):?> action="<?=h($url)?>"<?endif?>><?
}

function hidden($tag, $val)
{
?><input type="hidden" name="<?=h($tag)?>" value="<?=h($val)?>" /><?
}

function input($tag, $val, $width=20)
{
?><input type="text" name="<?=h($tag)?>" value="<?=h($val)?>" size="<?=h($width)?>' /><?
}

function textarea($tag, $val, $width=80, $height=10)
{
?><textarea name="<?=h($tag)?>" cols="<?=h($width)?>" rows="<?=h($height)?>">><?=h($val)?></textarea><?
}

function submit($tag, $val)
{
?><input type="submit" name="<?h($tag)?>" value="<?h($val)?>" /><?
}
?>
