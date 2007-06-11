(assert-check-true "regexp?"
                   (regexp? (string->regexp "\\d"))
                   (regexp? (string->regexp "abc"))
)

(assert-check-false "regexp?"
                   (regexp? 1)
                   (regexp? #\a)
                   (regexp? #t)
                   (regexp? "test")
                   (regexp? (current-input-port))
                   (regexp? (quote x))
                   (regexp? (cons 1 2))
)

(assert-check-true "match"
                   (rxmatch (string->regexp "\\d+") "345")
                   (rxmatch (string->regexp "\\w+") "abc")
                   (rxmatch (string->regexp "abc") "abcd")
)

(assert-check-true "match"
                   (rxmatch #/\d+/) "345")
                   (rxmatch #/\w+/) "abc")
                   (rxmatch #/abc/) "abcd")
)

(assert-check-false "unmatch"
                   (rxmatch (string->regexp "\\W+") "345")
                   (rxmatch (string->regexp "\\d+") "abc")
                   (rxmatch (string->regexp "def") "abcd")
)

(assert-check-true "match"
                   ((string->regexp "\\d+") "345")
                   ((string->regexp "\\w+") "abc")
                   ((string->regexp "abc") "abcd")
)

(assert-check-true "match"
                   (#/\d+/ "345")
                   (#/\w+/ "abc")
                   (#/abc/ "abcd")
)

(assert-check-false "unmatch"
                   ((string->regexp "\\W+") "345")
                   ((string->regexp "\\d+") "abc")
                   ((string->regexp "def") "abcd")
)

(assert-check-false "unmatch"
                   (#/\W+/) "345")
                   (#/\d+/) "abc")
                   (#/def/) "abcd")
)

(assert-check-equal string=?
                    '((rxmatch-substring (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) "abc123de")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) "abc123de")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) "123")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) "")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) "f")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test) "123")
                    '((rxmatch-substring (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test2) "f")
)

(assert-check-equal string=?
                    '((rxmatch-substring (rxmatch #/abc(\d+)de/ "abc123de")) "abc123de")
                    '((rxmatch-substring (rxmatch #/abc(\d+)de/ "abc123de") 0) "abc123de")
                    '((rxmatch-substring (rxmatch #/abc(\d+)de/ "abc123de") 1) "123")
                    '((rxmatch-substring (rxmatch #/abc(\d+)de(.*)/ "abc123de") 2) "")
                    '((rxmatch-substring (rxmatch #/abc(\d+)de(.*)/ "abc123def") 2) "f")
                    '((rxmatch-substring (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test) "123")
                    '((rxmatch-substring (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test2) "f")
)


(assert-check-equal =
                    '((rxmatch-start (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) 0)
                    '((rxmatch-start (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) 0)
                    '((rxmatch-start (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) 3)
                    '((rxmatch-start (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) 8)
                    '((rxmatch-start (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) 8)
                    '((rxmatch-start (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test) 3)
                    '((rxmatch-start (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test2) 8)
)

(assert-check-equal =
                    '((rxmatch-start (rxmatch #/abc(\d+)de/ "abc123de")) 0)
                    '((rxmatch-start (rxmatch #/abc(\d+)de/ "abc123de") 0) 0)
                    '((rxmatch-start (rxmatch #/abc(\d+)de/ "abc123de") 1) 3)
                    '((rxmatch-start (rxmatch #/abc(\d+)de(.*)/ "abc123de") 2) 8)
                    '((rxmatch-start (rxmatch #/abc(\d+)de(.*)/ "abc123def") 2) 8)
                    '((rxmatch-start (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test) 3)
                    '((rxmatch-start (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test2) 8)
)

(assert-check-equal =
                    '((rxmatch-end (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) 8)
                    '((rxmatch-end (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) 8)
                    '((rxmatch-end (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) 6)
                    '((rxmatch-end (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) 8)
                    '((rxmatch-end (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) 9)
                    '((rxmatch-end (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test) 6)
                    '((rxmatch-end (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test2) 9)
)

(assert-check-equal =
                    '((rxmatch-end (rxmatch #/abc(\d+)de/ "abc123de")) 8)
                    '((rxmatch-end (rxmatch #/abc(\d+)de/ "abc123de") 0) 8)
                    '((rxmatch-end (rxmatch #/abc(\d+)de/ "abc123de") 1) 6)
                    '((rxmatch-end (rxmatch #/abc(\d+)de(.*)/ "abc123de") 2) 8)
                    '((rxmatch-end (rxmatch #/abc(\d+)de(.*)/ "abc123def") 2) 9)
                    '((rxmatch-end (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test) 6)
                    '((rxmatch-end (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 'test2) 9)
)

(assert-check-equal string=?
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) "de")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 1) "def")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 2) "")
)

(assert-check-equal string=?
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) "de")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 1) "def")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 2) "")
)

(assert-check-equal string=?
                    '((rxmatch-before (rxmatch #/abc(\d+)de/ "abc123de")) "")
                    '((rxmatch-before (rxmatch #/abc(\d+)de/ "abc123de") 0) "")
                    '((rxmatch-before (rxmatch #/abc(\d+)de/ "abc123de") 1) "abc")
                    '((rxmatch-before (rxmatch #/abc(\d+)de(.*)/ "abc123de") 2) "abc123de")
                    '((rxmatch-before (rxmatch #/abc(\d+)de(.*)/ "abc123def") 2) "abc123de")
                    '((rxmatch-before (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 1) "abc")
                    '((rxmatch-before (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 2) "abc123de")
)

(assert-check-equal string=?
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) "de")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) "")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 1) "def")
                    '((rxmatch-after (rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 2) "")
)

(assert-check-equal string=?
                    '((rxmatch-after (rxmatch #/abc(\d+)de/ "abc123de")) "")
                    '((rxmatch-after (rxmatch #/abc(\d+)de/ "abc123de") 0) "")
;;                     '((rxmatch-after (rxmatch #/abc(\d+)de/ "abc123de") 1) "de")
;;                     '((rxmatch-after (rxmatch #/abc(\d+)de(.*)/ "abc123de") 2) "")
;;                     '((rxmatch-after (rxmatch #/abc(\d+)de(.*)/ "abc123def") 2) "")
;;                     '((rxmatch-after (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 1) "def")
;;                     '((rxmatch-after (rxmatch #/abc(?<test>\d+)de(?<test2>.*)/ "abc123def") 2) "")
)

(assert-check-equal string=?
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'before) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'before 0) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'before 1) "abc")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 'before 2) "abc123de")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 'before 2) "abc123de")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'before 1) "abc")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'before 2) "abc123de")
)

(assert-check-equal string=?
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'after) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'after 0) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 'after 1) "de")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 'after 2) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 'after 2) "")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'after 1) "def")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'after 2) "")
)

(assert-check-equal string=?
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de")) "abc123de")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 0) "abc123de")
                    '(((rxmatch (string->regexp "abc(\\d+)de") "abc123de") 1) "123")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123de") 2) "")
                    '(((rxmatch (string->regexp "abc(\\d+)de(.*)") "abc123def") 2) "f")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test) "123")
                    '(((rxmatch (string->regexp "abc(?<test>\\d+)de(?<test2>.*)") "abc123def") 'test2) "f")
)

(assert-check-equal =
                    '((rxmatch-num-matches (rxmatch (string->regexp "abc(\\d+)de") "abc123de")) 2)
                    '((rxmatch-num-matches (rxmatch (string->regexp "abc(\\d+)de") "hoge")) 0)
)

(assert-check-false "false pattern"
                    (rxmatch-substring #f)
                    (rxmatch-after #f)
                    (rxmatch-before #f)
                    (rxmatch-start #f)
                    (rxmatch-end #f)
)
