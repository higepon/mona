
monaSq 1.0


■説明

プログラム言語 Squirrel を MonaOS で実行するプログラムです。
MonaOS 0.3.0 alpha5 にて動作確認しました。



■Squirrel について

Squirrel は C 言語に似た構文を持ち Lua 言語の影響を受けた、
いわゆる LightWegight な組み込み用スクリプト言語です。

オブジェクト指向、例外、ガベージコレクション、クロージャ、マイクロスレッドなどを
サポートしています。
詳しくは、以下のＵＲＬを参照してください。

Squirrel 公式ページ（英語）
http://squirrel-lang.org/

Squirrel 2.1 リファレンスマニュアル（日本語）
http://muffin.cias.osakafu-u.ac.jp/~matumoto/cgi-bin/xt.cgi?prog/squirrel_lang


Squirrel は『スクイレル』または『スクイール』と読み、英語でリスを意味します。
ソースファイル拡張子は ".nut" （木の実の意。".sq"ではないことに注意）、
バイトコードコンパイル済みファイル拡張子は ".cnut"です。



■操作方法

MonaSq のウインドウは、ボタン、入力エリア、出力エリアの
３つに分かれています。

入力エリアにスクリプトを入力して、
『実行』ボタンをクリックするか『Alt+Enter』キーを押すと
スクリプトが実行されます。


ボタン操作：

  ↑ボタン      前のスクリプト実行履歴を表示します。

  ↓ボタン      次のスクリプト実行履歴を表示します。

  入力欄クリア  入力エリアの内容を消去します。

  出力欄クリア  出力エリアの内容を消去します。

  実行          入力エリアのスクリプトを実行します。
                実行中は「停止」ボタンになり、
                クリックするとスクリプトの実行を停止します。

キー操作：

  Alt+Enter     入力エリアのスクリプトを実行します。

  Ctrl+Enter    入力エリアの現在行のスクリプトを式として評価して、
                出力エリアに結果を表示します。
                例：
                  1 + 1 <Ctrl+Enter>
                出力：
                  2

  Ctrl+Tab      入力エリア←→出力エリア間でカーソルを行き来します。

  Alt+↑        前の履歴を表示します。
  
  Alt+↓        次の履歴を表示します。



MonaSq は、起動時に一度だけ Squirrel 実行環境を初期化します。
スクリプトは実行ボタンを押すたびに実行されますが、
一度定義したグローバル変数や関数は次の実行時にも参照することができます。

スクリプト履歴には、一度実行したスクリプトが保存されます。
起動直後は、あらかじめ用意されたサンプルスクリプトが履歴に入っています。
入力したスクリプトをファイルなどに保存する機能はありません。



■ビルド方法

MonaADK (Mona Application Development Kit) を使用しています。
ビルドの際は、MonaADK の z_tools フォルダと同じディレクトリに
このプロジェクトのフォルダを置いて、コマンドプロンプトを起動してください。


ビルド→実行手順
  make
  make run

デバッグ版のビルド→実行手順
  make DEBUG=TRUE
  make run

オブジェクトファイルの削除
  make clean


デバッグ版は開発用です。
コンソールエリアに余計なデバッグ情報が表示されます。



■ファイル構成

複数のソースコードライセンスが混在しています。
利用させてもらったソースコード内で、MonaSq 用に改変した部分は
#if defined( MONASQ )
#endif
で囲んであります。


monasq.cpp
monasq.h
monasq_base.h
monasq_baygui.cpp
monasq_monapi.cpp
init_script.cpp
  このプログラムのメインルーチン。
  MIT Licence です。

doc/
  Squirrel 言語と Squirrel 標準ライブラリのマニュアル（英語）。

sq/
  Squirrel のソースコード。
  zlib/libpng licence です。
    squirrel 2.1.1 stable
    http://squirrel-lang.org/

sqplus/
  Squirrel 用のヘルパーライブラリ。
  ライセンスは分かりませんが、sqplus.h に「Free for any use」とあります。
    SQUIRREL2_1_1_sqplus_23
    http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SqPlus.html

misc/setjmp.h
misc/setjmp.cpp
  monalibc の longjmp() 関数を呼び出すと
  ハングアップする現象に遭遇したため、独自に定義してあります。
  以下から拝借して、gccでコンパイルが通るように修正しました。
  この２ファイルは 修正 BSD licence です。
    Sanos Operating System Kernel
    http://www.jbox.dk/sanos/index.htm

misc/vsprintf.cpp
misc/fcvt.cpp
  monalibc の vsprintf() は書式が一部未対応なので
  独自に定義してあります。
  以下から拝借しました。
  この２ファイルは 修正 BSD licence です。
    Sanos Operating System Kernel
    http://www.jbox.dk/sanos/index.htm

misc/TextArea.h
misc/TextArea.cpp
  baygui/TextArea.h をインクルードしても
  リンカに参照が見つからないと怒られる現象に遭遇したため、
  独自に定義してあります（現在の libbaygui.a には含まれていないっぽい？）。
  以下を元に、少し改変してあります。
    /trunk/contrib/Graphics/libbaygui/awt/TextArea.cpp
    Revision 3120



■バグ

・ハングアップバグ
  現象：
    bayguiコンポーネントが使用中なのに破棄されることがある
  詳細：
    MonaSq が参照カウントを考慮していないため、
    Squirrel 変数がスコープを出ると bayguiコンポーネントであっても
    GC に回収されてしまいます。その結果、無効なポインタに
    対するアクセスが発生し MonaSq はハングアップします。
  対処方法：
    泥縄的ですが、bayguiコンポーネントは使用期間以上の変数スコープで
    保持するようにしてください。



■MonaOS 0.3.0 alpha5 上でのアプリ開発時の問題点とその対策

monaSq を開発する上で発生した、
現状の MonaOS 固有と思われる問題点とその対策についてのメモです。


・stdout, stdin が未定義
  →とりあえず NULL で代用

・ファンクションキーやBREAKキーなどが送られてこない
  →諦めて他のキーで代用

・CheckBox::CheckBox() 呼び出しがリンクできない
  CheckBox::CheckBox("") なら呼び出せるのでそちらで代用

・C++例外が使えない
  →その場で exit(-1) してしまうか、エラーを無視するように修正

・placement new のデフォルトハンドラが未定義
  →定義して対応
  ※sq/squirrel/sqpcheader.h を参照

・setjmp / longjmp はハングアップすることがある
  →言語系のプログラムでは致命的なので、別の setjmp 実装を借用して代用

・printf 系関数で %f や %x のような書式が無視される
  →monalibc 対応待ちのつもりだったが、ついでなので setjmp とともに借用
    （但しこの実装には snprintf() 系がない）

・TextArea クラスがリンクできない
  →複数行テキストが使えないとこのアプリが成り立たないので、
    MonaOS のレポジトリからソースを借用して代用

