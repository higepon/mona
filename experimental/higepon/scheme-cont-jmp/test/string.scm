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
