<?php header('Content-Type: text/html; charset=EUC-JP') ?>
<?php echo '<?xml version="1.0" encoding="EUC-JP"?>' ?>

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
 <link rel="stylesheet" href="/dynamic/website/skin/theme/cool_ice/cool_ice.css" type="text/css" media="all" />
 <link rel="stylesheet" href="/dynamic/website/skin/pukiwiki.css" type="text/css" media="all" />
</head>
<body>

<?php if (arg_check('read') and is_page('MenuBar')) { ?>
<div class="main">
<?php } ?>

<div class="adminmenu">
	<span class="adminmenu">
<?php if ($is_page) { ?>
 [ <a href="<?php echo "$script?$r_page" ?>">リロード</a> ]
 &nbsp;
 [ <a href="<?php echo "$script?plugin=newpage" ?>">新規</a>
 | <a href="<?php echo $link_edit ?>">編集</a>
 
 <?php   if ($is_read and $function_freeze) { ?>
<?php     if ($is_freeze) { ?>
 | <a href="<?php echo $link_unfreeze ?>">凍結解除</a>
<?php     } else { ?>
 | <a href="<?php echo $link_freeze ?>">凍結</a>
<?php     } ?>
<?php   } ?>

 | <a href="<?php echo $link_diff ?>">差分</a>

<?php   if ((bool)ini_get('file_uploads')) { ?>
 | <a href="<?php echo $link_upload ?>">添付</a>
<?php   } ?>

 ]
 &nbsp;<br />
<?php } ?>

 [ <a href="<?php echo $link_top ?>">トップ</a>
 | <a href="<?php echo $link_list ?>">一覧</a>

<?php if (arg_check('list')) { ?>
 | <a href="<?php echo $link_filelist ?>">ファイル名一覧</a>
<?php } ?>

 | <a href="<?php echo $link_search ?>">単語検索</a>
 | <a href="<?php echo $link_whatsnew ?>">更新履歴</a>
 | <a href="<?php echo $link_rss ?>">RSS</a>

<?php if ($do_backup) { ?>
 | <a href="<?php echo $link_backup ?>">バックアップ</a>
<?php } ?>

 | <a href="<?php echo $link_help ?>">ヘルプ</a>
 ]
	</span>
</div>

 <h1 class="title"><?php echo $page ?></h1>

<div class="day">
<div class="body">
<div class="section">
<?php echo $body ?>

<?php if ($notes) { ?>
<?php echo $notes ?>
<?php } ?>

<?php if ($attaches) { ?>
<?php echo $hr ?>
<?php echo $attaches ?>
<?php } ?>

</div>
</div>
</div>

<div class="footer">
<?php if ($lastmodified) { ?>
 Last-modified: <?php echo $lastmodified ?><br />
 <?php } ?>
 <?php if ($related) { ?>
 Link: <?php echo $related ?>
<?php } ?><br />
Modified by <a href="<?php echo $modifierlink ?>"><?php echo $modifier ?></a>
 <br />
 <?php echo S_COPYRIGHT ?>
 <br />
 Powered by PHP <?php echo PHP_VERSION ?>
 <br />
 HTML convert time to <?php echo $taketime ?> sec.
</div>

<?php if (arg_check('read') and is_page('MenuBar')) { ?>
</div>
<div class="sidebar">
<?php echo preg_replace('/<ul[^>]*>/','<ul>',convert_html(get_source('MenuBar'))) ?>
</div>
<?php } ?>

</body>
</html>
