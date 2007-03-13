(assert-check-true "procedure +"
                   (= 3 (+ 1 2))
                   (= 2 (+ 4 -2))
                   (= 11 (+ 4 2 5)))

(assert-check-true "procedure -"
                   (= 2 (- 3 1))
                   (= 0 (- 3 1 2))
                   (= -3 (- 3 1 2 3))
                   (= -3 (- 3)))

(assert-check-true "procedure /"
                   (= 2 (/ 4 2))
                   (= 2 (/ 8 2 2))
                   (= 1 (/ 54 2 9 3)))

(assert-check-true "procedure *"
                   (= 2 (* 1 2))
                   (= -8 (* 4 -2))
                   (= 40 (* 4 2 5)))

(assert-check-true "number? should be #t"
                   (number? 3)
                   (number? (+ 1 2)))

(assert-check-false "number? should be #f"
                    (number? "string")
                    (number? (cons 1 2)))

(assert-check-true "="
                   (= 1 1 1 1)
                   (= 1 (+ 0 1) 1)
                   (= 1 1))

(assert-check-false "="
                   (= 1 2 1 1)
                   (= 1 "1" 1))

(assert-check-true ">"
                   (> 2 1)
                   (> 5 4 3)
                   (> 5 4 3 2 1))

(assert-check-false ">"
                   (> 1 2)
                   (> 5 3 4)
                   (> 5 4 4)
                   (> "4" 3 2 1))

(assert-check-true ">="
                   (>= 2 1)
                   (>= 5 4 4)
                   (>= 5 4 3 2 1))

(assert-check-false ">="
                   (>= 1 2)
                   (>= 5 3 4)
                   (>= "4" 3 2 1))

(assert-check-false "< #f"
                   (< 2 1)
                   (< 5 4 3)
                   (< 5 4 3 2 1))

(assert-check-true "<"
                   (< 1 2)
                   (< 5 6 7)
                   (< 1 2 3 4))

(assert-check-false "<= #f"
                   (<= 2 1)
                   (<= 5 4 4)
                   (<= 5 4 3 2 1))

(assert-check-true "<="
                   (<= 1 2)
                   (<= 3 4 4))

(assert-check-true "number->string"
                   (string=? (number->string 1234) "1234"))

(assert-check-true "positive?/negative?"
                   (positive? 1)
                   (negative? -2))

(assert-check-false "positive?/negative?"
                   (positive? 0)
                   (positive? -1)
                   (negative? 0)
                   (negative? 1))

(assert-check-true "abs"
                   (= (abs 3) 3)
                   (= (abs 0) 0)
                   (= (abs -1) 1))

(assert-check-true "not"
                   (if (not #f) #t #f)
                   (if (not 3)  #f #t))

(assert-check-true "odd?/even?"
                   (odd? 1)
                   (odd? 3)
                   (odd? 5)
                   (odd? 7)
                   (even? 0)
                   (even? 2)
                   (even? 4)
                   (even? 6))

(assert-check-true "complex? / real? / rational? /integer?"
                   (complex? 3)
                   (real? 3)
                   (rational? 3)
                   (integer? 3))

(assert-check-false "complex? / real? / rational? /integer? should be #f"
                   (complex? "abc")
                   (real? (cons 1 2))
                   (rational? #\a)
                   (integer? (current-input-port)))

(assert-check-true "exact?"
                   (exact? 3)
                   (exact? -2))

(assert-check-false "exact? should be #f"
                   (exact? #\a)
                   (exact? "a")
                   (exact? (cons 1 2)))

(assert-check-false "inexact? should be #f"
                   (inexact? 3)
                   (inexact? #\a)
                   (inexact? "a")
                   (inexact? (cons 1 2)))

(assert-check-true "quotient"
                   (= (quotient 5 3) 1)
                   (= (quotient 6 3) 2)
                   (= (quotient 7 3) 2))

(assert-check-true "modulo/reminder"
                   (= (modulo 13 4) 1)
                   (= (modulo -13 4) 1)
                   (= (modulo 13 -4) 1)
                   (= (reminder 13 4) 1)
                   (= (reminder -13 4) -1)
                   (= (reminder 13 -4) 1)
                   (= (reminder -13 -4) -1))

(assert-check-true "max / min"
                   (= 4 (max 1 2 4 3))
                   (= 1 (min 2 1 4 3)))

(assert-check-true "gcd / lcm"
                   (= (gcd 32 -36) 4)
                   (= (gcd 36 12) 12)
                   (= (gcd 1 1) 1)
                   (= (lcm 2 1) 2)
                   (= (lcm 3 4) 12)
                   (= (lcm 12 9) 36
))
