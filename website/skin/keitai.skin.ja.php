<?php
if (!defined('DATA_DIR')) { exit; }

global $script,$vars,$page_title,$max_size,$accesskey;

header('Content-Type: text/html; charset=Shift_JIS');

//1KByte余裕を見る(ヘッダなど)
$max_size = (--$max_size * 1024);

// エンコード
$title = mb_convert_encoding($title,'SJIS',SOURCE_ENCODING);
$body = mb_convert_encoding($body,'SJIS',SOURCE_ENCODING);

// 画像を置換
$body = preg_replace('#(<div[^>]+>)?(<a[^>]+>)?<img[^>]*alt="([^"]+)"[^>]*>(?(2)</a>)(?(1)</div>)#','$3',$body);

// ページ番号
$pageno = (isset($vars['p']) and is_numeric($vars['p'])) ? $vars['p'] : 0;
$pagecount = ceil(strlen($body) / $max_size);
$lastpage = $pagecount - 1;
$r_page = rawurlencode($vars['page']);

// ナビゲーション
$navi = array();
$navi[] = "<a href=\"$link_top\" $accesskey=\"0\">0.Top</a>";
$navi[] = "<a href=\"$script?plugin=newpage&amp;refer=$r_page\" $accesskey=\"1\">1.New</a>";
$navi[] = "<a href=\"$link_edit\" $accesskey=\"2\">2.Edit</a>";
if ($is_read and $function_freeze) {
	if ($is_freeze) {
		$navi[] = "<a href=\"$link_unfreeze\" $accesskey=\"3\">3.Unfreeze</a>";
	}
	else {
		$navi[] = "<a href=\"$link_freeze\" $accesskey=\"3\">3.Freeze</a>";
	}
}
$navi[] = "<a href=\"$script?MenuBar\" $accesskey=\"4\">4.Menu</a>";
$navi[] = "<a href=\"$link_whatsnew\" $accesskey=\"5\">5.Recent</a>";

// 前/次のブロック
if ($pagecount > 1) {
	$prev = $pageno - 1;
	$next = $pageno + 1;
	if ($pageno > 0) {
		$navi[] = "<a href=\"$script?cmd=read&amp;page=$r_page&amp;p=$prev\" $accesskey=\"7\">7.Prev</a>";
	}
	$navi[] = "$next/$pagecount ";
	if ($pageno < $lastpage) {
		$navi[] = "<a href=\"$script?cmd=read&amp;page=$r_page&amp;p=$next\" $accesskey=\"8\">8.Next</a>";
	}
}
$navi = join(' | ',$navi);
$body = substr($body,$pageno * $max_size, $max_size);
?>
<html>
<head><title><?php echo $title ?></title></head>
<body>
<?php echo $navi ?><hr><?php echo $body ?>
</body>
</html>
