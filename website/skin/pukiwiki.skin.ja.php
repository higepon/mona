<?php
if (!defined('DATA_DIR')) { exit; }
header('Cache-control: no-cache');
header('Pragma: no-cache');
header('Content-Type: text/html; charset=EUC-JP');
/**
// XML宣言を吐くと標準モードじゃなくなるIE・・・
echo '<?xml version="1.0" encoding="EUC-JP"? >';
// ? > ←注意
/**/
?>

<?php if ($html_transitional) { ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<?php } else { ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja">
<?php } ?>
<head>
 <meta http-equiv="content-type" content="application/xhtml+xml; charset=EUC-JP" />
 <meta http-equiv="content-style-type" content="text/css" />

<?php if (!$is_read) { ?>
 <meta name="robots" content="NOINDEX,NOFOLLOW" />
<?php } ?>

 <title><?php echo "$title - $page_title" ?></title>
 <link rel="stylesheet" href="../../website/skin/default.ja.css" type="text/css" media="screen" charset="Shift_JIS" />
 <link rel="stylesheet" href="../../website/skin/print.ja.css" type="text/css" media="print" charset="Shift_JIS" />
<?php
  global $trackback, $referer;
  if ($trackback) {
?>
 <meta http-equiv="Content-Script-Type" content="text/javascript" />
 <script type="text/javascript" src="skin/trackback.js"></script>
<?php } ?>
<?php echo $head_tag ?>
</head>
<body>

<div id="Header"><a href="http://mona.sourceforge.jp/"><img src="./img/MonaLogo.gif" width="320px" height="50px" alt="Mona OS のロゴ" title="Mona logo"></a></div>
<div id="Menu">
<?php if ($is_page) { ?>
<a href="<?php echo "$script?$r_page" ?>">リロード</a> | 
<a href="<?php echo "$script?plugin=newpage&amp;refer=$r_page" ?>">新規</a> | 
<a href="<?php echo $link_edit ?>">編集</a> | 
<?php   if ($is_read and $function_freeze) { ?>
<?php     if ($is_freeze) { ?>
<a href="<?php echo $link_unfreeze ?>">凍結解除</a> | 
<?php     } else { ?>
<a href="<?php echo $link_freeze ?>">凍結</a> | 
<?php     } ?>
<?php   } ?>
<a href="<?php echo $link_diff ?>">差分</a> | 
<?php   if ((bool)ini_get('file_uploads')) { ?>
<a href="<?php echo $link_upload ?>">添付</a> | 
<?php   }} ?>
<a href="<?php echo $link_top ?>">トップ</a> | 
<a href="<?php echo $link_list ?>">一覧</a> | 
<?php if (arg_check('list')) { ?>
<a href="<?php echo $link_filelist ?>">ファイル名一覧</a> | 
<?php } ?>
<a href="<?php echo $link_search ?>">単語検索</a> | 
<a href="<?php echo $link_whatsnew ?>">最終更新</a> | 
<?php if ($do_backup) { ?>
<a href="<?php echo $link_backup ?>">バックアップ</a> | 
<?php } ?>
<a href="<?php echo $link_help ?>">ヘルプ</a>
<?php
  if ($trackback) {
    $tb_id = tb_get_id($_page);
?>
 | <a href="<?php echo "$script?plugin=tb&amp;__mode=view&amp;tb_id=$tb_id" ?>">TrackBack(<?php echo tb_count($_page) ?>)</a>
<?php } ?>
<?php
  if ($referer) {
?>
 | <a href="<?php echo "$script?plugin=referer&amp;page=$r_page" ?>">リンク元</a>
<?php } ?>
</div>

<hr class="Start" title="以下はこのページのタイトルです">

 <h1 id="title"><?php echo $page ?></h1>

<?php if ($is_page) { ?>
 <p class="PageLink"><a href="<?php echo "$script?$r_page" ?>"><?php echo "$script?$r_page" ?></a><p>
<?php } ?>

<p class="Attention"><strong>このWikiは <a href="http://wiki.monaos.org/">wiki.monaos.org</a> へ移転しました</strong></p>
<p class="Attention"><strong>お探しのページは<a href="http://wiki.monaos.org/pukiwiki.php?<?php print(urlencode($title)); ?>">こちら</a>かと思われます</strong></p>
<p class="Attention"><strong><small>以後このWiki（mona.sourceforge）を編集しても意味がありませんのでご注意願います。</small></strong></p>

<hr class="Start" title="以下はこのページのメインコンテンツです">

<?php if (arg_check('read') and exist_plugin_convert('menu')) { ?>
<div id="Contents">
<div id="ContentsF"><?php echo $body ?></div>
<div id="Panels"><?php echo do_plugin_convert('menu') ?></div>
</div>
<?php } else { ?>
<div id="Contents"><?php echo $body ?></div>
<?php } ?>

<?php if ($notes) { ?>
<div id="note">
<?php echo $notes ?>
</div>
<?php } ?>


<?php if ($attaches) { ?>
<div id="attach">
<hr class="Start" title="以下は添付ファイルです" />
<?php echo $attaches ?>
</div>
<?php } ?>

<hr class="Start" title="" />

<div id="toolbar">

<?php if ($is_page) { ?>
 <a href="<?php echo "$script?$r_page" ?>"><img src="./image/reload.png" width="20" height="20" alt="リロード" title="リロード" /></a>
 &nbsp;
 <a href="<?php echo $script ?>?plugin=newpage"><img src="./image/new.png" width="20" height="20" alt="新規" title="新規" /></a>
 <a href="<?php echo $link_edit ?>"><img src="./image/edit.png" width="20" height="20" alt="編集" title="編集" /></a>
 <a href="<?php echo $link_diff ?>"><img src="./image/diff.png" width="20" height="20" alt="差分" title="差分" /></a>
<?php   if ((bool)ini_get('file_uploads')) { ?>
 <a href="<?php echo $link_upload ?>"><img src="./image/file.png" width="20" height="20" alt="添付" title="添付" /></a>
<?php   } ?>
 &nbsp;
<?php } ?>
 <a href="<?php echo $link_top ?>"><img src="./image/top.png" width="20" height="20" alt="トップ" title="トップ" /></a>
 <a href="<?php echo $link_list ?>"><img src="./image/list.png" width="20" height="20" alt="一覧" title="一覧" /></a>
 <a href="<?php echo $link_search ?>"><img src="./image/search.png" width="20" height="20" alt="検索" title="検索" /></a>
 <a href="<?php echo $link_whatsnew ?>"><img src="./image/recentchanges.png" width="20" height="20" alt="最終更新" title="最終更新" /></a>
<?php if ($do_backup) { ?>
 <a href="<?php echo $link_backup ?>"><img src="./image/backup.png" width="20" height="20" alt="バックアップ" title="バックアップ" /></a>
<?php } ?>
 &nbsp;
 <a href="<?php echo $link_help ?>"><img src="./image/help.png" width="20" height="20" alt="ヘルプ" title="ヘルプ" /></a>
 &nbsp;
 <a href="<?php echo $link_rss ?>"><img src="./image/rss.png" width="36" height="14" alt="最終更新のRSS" title="最終更新のRSS" /></a>
</div>

<hr class="Start" title="以下はこのページに関するいくつかの情報です">

<div id="modify">
<?php if ($lastmodified) { ?>
Last-modified: <?php echo $lastmodified ?>;&nbsp;
<?php } ?>
Modified by <a href="<?php echo $modifierlink ?>"><?php echo $modifier ?></a>
</div>
<?php if ($related) { ?>
<div id="related">Link: <?php echo $related ?></div>
<?php } ?>
<div id="versioninfo"><?php echo S_COPYRIGHT ?><br />Powered by PHP <?php echo PHP_VERSION ?></div>
<div id="converttime">HTML convert time to <?php echo $taketime ?> sec.</div>

<hr class="Start" title="以下は著作権表記です">

<div id="LinkSF"><a href="http://sourceforge.jp"><img src="http://sourceforge.jp/sflogo.php?group_id=320" width="96px" height="31px" alt="SourceForgeへのリンク" title="MonaOSはSourceFORGE.jpの提供で運営しております"></a></div>
<div id="Copyright">Copyright(C)2003-2005 MonaProjectTeam, all rights reserved. powered by www.be-interactive.org</div>

</body>
</html>
