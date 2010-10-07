;; 第1回 Scheme コードバトン
;;
;; ■ これは何か？
;;   Scheme のコードをバトンのように回していき面白い物ができあがるのを楽しむ遊びです。
;;   次回 Shibuya.lisp で成果を発表します。
;;   Scheme 初心者のコードを書くきっかけに、中級者には他人のコードを読む機会になればと思います。
;;
;; ■ 2 つのルール
;;
;;   (1)自分がこれだと思える変更をコードに加えて2日以内に次の人にまわしてください。
;;      「人に優しい」変更なら何でも良い。1文字の変更でも可。
;;      「人に優しい」とは例えば、次の人が読みやすいコードを書くなど。
;;       コードを削るのもあり。
;;
;;   (2)次の人にまわしコードが変更されるのを"見守る"。
;;       この説明書きを含めてバトンが伝わった事を必ず確認してください。
;;       止まっていたら助けてあげてください。
;;
;; ■ バトンの回し方
;;
;;    (1) 回ってきたバトンは http://gist.github.com/xxxx という URL のはずです。
;;    (2) fork をクリックしてください（アカウントを持っていない人はこのとき作成します）
;;    (3) edit で変更したファイルを貼り付けます。
;;    (4) 自分が fork した新しい URL を回してください
;;
;;
;; ■ 良くある質問
;;
;;   (a) 初心者です。参加したいけどちょっと不安です。
;;      higepon がフォローしますので大丈夫です。分からない事があれば遠慮無く聞いてください。
;;
;;   (b) 次にまわす人がいません
;;      higepon に知らせてください。twitter, 日記のコメントなどで。
;;
;;   (c) 次の人がコードを止めてしまいました
;;      残念ですが別の人にバトンを渡してください。
;;
;;   (d) Mosh で動かないとダメですか？
;;      いいえ。Scheme なら何でも良いです。Gauche, Ypsilon 用に書き換えるのも面白いですね。
;;      そのときは起動方法の説明も変えてください。
;;
;; ■ バトンの行方を記録
;;   名前(URL)：一言
;;   1. higepon (http://d.hatena.ne.jp/higepon/): 最初はコマンドライン英単語暗記ツールでした。これが何に化けるのか楽しみ。全く別物になるかな？

;; =================================================================================================================================================
;;                            これより下がコードとその説明 - 変更・削除歓迎
;; =================================================================================================================================================

;; ■英単語暗記補助ツールです
;;   起動すると辞書ファイルから単語が表示されるので意味を頭で考えます。Ctrl-D を押すと答えが表示されます。 (y/n) を聞かれるので正解なら y を押してください。
;;   間違った単語は辞書ファイルに記録され次回出題されます。
;;
;; ■動作方法
;;   Mosh (0.2.0) で動作します。(http://code.google.com/p/mosh-scheme/downloads/list)
;;   % mosh scheme_button.scm 辞書ファイル
;;
;; ■辞書ファイルの例
;;   http://gist.github.com/273424

(import (rnrs)
        (mosh control)
        (mosh)
        (match)
        (srfi :8)
        (only (srfi :1) first second))

;; ファイルを読んで S 式のリストにする
(define (file->sexp-list file)
  (with-input-from-file file
    (lambda ()
      (let loop ([line (read)]
                 [ret '()])
        (cond
         [(eof-object? line) (reverse ret)]
         [else
          (loop (read) (cons line ret))])))))

;; 辞書ファイルをパース
(define (sort-word-spec* word-spec*)
  (list-sort
   ;; 間違いが多い順にソート
   (lambda x
     (match x
       [((_a _b ok-count1 ng-count1) (_c _d ok-count2 ng-count2))
        (> (- ng-count1 ok-count1) (- ng-count2 ok-count2))]))
   ;; 辞書形式は (word meaning) または (word meaning ok-count ng-count)
   (map (lambda (word-spec*)
          (match word-spec*
            [(word meaning)
             (list word meaning 0 0)]
            [(word meaning ok-count ng-count)
             (list word meaning ok-count ng-count)]))
        word-spec*)))

(define (main args)
  (let1 result*
      (call/cc
       (lambda (break)
         (let loop ([word-spec* (sort-word-spec* (file->sexp-list (second args)))]
                    [result-spec* '()])
           (match word-spec*
             [() result-spec*]
             [((word meaning ok-count ng-count) . more)
              ;; 問題出題
              (format #t "\n~s: " word)
              ;; Ctrl-D 待ち
              (read (current-input-port))
              ;; 答え表示
              (format #t "~s y/n? " meaning)
              (let1 c (read-char (current-input-port))
              (case c
                ;; Y だったら
                [(#\y #\Y)
                 (loop more (cons (list word meaning (+ ok-count 1) ng-count) result-spec* ))]
                ;; N だったら
                [(#\N #\n)
                 (loop more (cons (list word meaning ok-count (+ ng-count 1)) result-spec*))]
                ;; Q だったら途中でやめるので成績を記録
                [(#\q #\Q)
                 (break (append (reverse result-spec*) word-spec*))]
                [else
                 (format #t "bug=<~a>" c)]))]))))
    ;; 正答と誤答を記録
    (call-with-port (open-file-output-port (second args) (make-file-options '(no-fail)) 'block (native-transcoder))
                    (lambda (p)
                      (for-each (lambda (x)
                                  (write x p)
                                  (write x)
                                  (newline p)) result*)))))

(main (command-line))
