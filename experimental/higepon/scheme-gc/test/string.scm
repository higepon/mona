(assert-check-false "string?"
                    (string? 3)
                    (string? (cons 1 2))
                    (string? (+ 1 2)))
(assert-check-true "string?" (string? "string"))

(assert-check-true "string->number" (= 1234 (string->number "1234")))
(assert-check-true "string-append" (string=? (string-append "12" "34") "1234"))

(assert-check-true "make-string"
                   (string=? "1111" (make-string 4 #\1)))

(assert-check-true "string-length"
                   (= (string-length "abcdef") 6))

(assert-check-true "string-ref"
                   (char=? (string-ref "abcdef" 2) #\c))

(let ((str "abcdef"))
  (assert-check-true "string-set!"
                     (string-set! str 2 #\z)
                     (string=?  str "abzdef")))

(assert-check-true "string<?"
                   (string<? "abc" "abcd")
                   (string<? "a" "ab")
                   (string<? "abv" "xyz")
)

(assert-check-false "string<? should be #f"
                   (string<? "abcd" "abc")
                   (string<? "ab" "a")
)

(assert-check-true "string>?"
                   (string>? "abcd" "abc")
                   (string>? "ab" "a")
                   (string>? "xyz" "ab")
)

(assert-check-false "string>? should be #f"
                   (string>? "abc" "abcd")
                   (string>? "a" "b")
)

(assert-check-true "string<=?"
                   (string<=? "abc" "abcd")
                   (string<=? "abc" "abc")
                   (string<=? "a" "ab")
                   (string<=? "abv" "xyz")
)

(assert-check-false "string<=? should be #f"
                   (string<=? "abcd" "abc")
                   (string<=? "ab" "a")
)

(assert-check-true "string>=?"
                   (string>=? "abcd" "abc")
                   (string>=? "abcd" "abcd")
                   (string>=? "ab" "a")
                   (string>=? "xyz" "ab")
)

(assert-check-false "string>=? should be #f"
                   (string>=? "abc" "abcd")
                   (string>=? "a" "b")
)

(assert-check-true "string-ci<?"
                   (string-ci<? "abc" "abcd")
                   (string-ci<? "a" "ab")
                   (string-ci<? "abv" "xyz")
                   (string-ci<? "abC" "ABCD")
                   (string-ci<? "a" "Ab")
                   (string-ci<? "abv" "XYz")
)

(assert-check-false "string-ci<? should be #f"
                   (string-ci<? "abcd" "abc")
                   (string-ci<? "ab" "a")
                   (string-ci<? "Abcd" "abc")
                   (string-ci<? "Ab" "A")
)

(assert-check-true "string-ci>?"
                   (string-ci>? "abcD" "abc")
                   (string-ci>? "Ab" "a")
                   (string-ci>? "xYz" "ab")
)

(assert-check-false "string-ci>? should be #f"
                   (string-ci>? "abc" "abcd")
                   (string-ci>? "a" "B")
                   (string-ci>? "Abc" "abcd")
                   (string-ci>? "a" "b")
)

(assert-check-true "string-ci<=?"
                   (string-ci<=? "abc" "abcd")
                   (string-ci<=? "abc" "abc")
                   (string-ci<=? "a" "ab")
                   (string-ci<=? "abv" "xyz")
                   (string-ci<=? "abc" "Abcd")
                   (string-ci<=? "abc" "aBc")
                   (string-ci<=? "a" "Ab")
                   (string-ci<=? "abv" "Xyz")
)

(assert-check-false "string-ci<=? should be #f"
                   (string-ci<=? "abcd" "abc")
                   (string-ci<=? "ab" "a")
                   (string-ci<=? "Abcd" "aBc")
                   (string-ci<=? "Ab" "A")
)

(assert-check-true "string-ci>=?"
                   (string-ci>=? "abcd" "abc")
                   (string-ci>=? "abcd" "abcd")
                   (string-ci>=? "ab" "a")
                   (string-ci>=? "xyz" "ab")
                   (string-ci>=? "Abcd" "abc")
                   (string-ci>=? "Abcd" "abcd")
                   (string-ci>=? "aB" "a")
                   (string-ci>=? "xYz" "ab")
)

(assert-check-false "string-ci>=? should be #f"
                   (string-ci>=? "abc" "abcd")
                   (string-ci>=? "a" "b")
                   (string-ci>=? "abc" "abcD")
                   (string-ci>=? "a" "B")
)

(assert-check-true "string-ci=?"
                   (string-ci=? "abc" "abc")
                   (string-ci=? "aBc" "abc")
                   (string-ci=? "abc" "Abc")
                   (string-ci=? "ABC" "abc")
)

(assert-check-false "string-ci=? should be #f"
                   (string-ci=? "abC" "abcD")
                   (string-ci=? "abc" "abcd")
)

(assert-check-true "substring"
                   (string=? (substring "12345" 2 5) "345")
                   (string=? (substring "12345" 0 1) "1")
                   (string=? (substring "12345" 0 5) "12345")
)

(let ((l (string->list "abc")))
  (assert-check-true "string->list"
                     (= (length l) 3)
                     (char=? (list-ref l 0) #\a)
                     (char=? (list-ref l 1) #\b)
                     (char=? (list-ref l 2) #\c)
))

(let ((s (list->string (list #\1 #\2 #\3))))
  (assert-check-true "list->string"
                     (string=? s "123"))
)

(assert-check-true "string-copy"
                   (string=? (string-copy "1234") "1234"))

(let ((s "1234"))
  (string-fill! s #\A)
  (assert-check-true "string-fill!"
                     (string=? s "AAAA")))

(assert-check-true "string"
                   (string=? (string #\a #\b) "ab"))
