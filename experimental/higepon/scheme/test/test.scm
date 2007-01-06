(load "./test/scheme.scm")
(load "./test/unittest.scm")

(define test-name "procedure +")
(assert-true test-name (= 3 (+ 1 2)))
(assert-true test-name (= 2 (+ 4 -2)))
(assert-true test-name (= 11 (+ 4 2 5)))

(define test-name "procedure -")
(assert-true test-name (= 2 (- 3 1)))
(assert-true test-name (= 0 (- 3 1 2)))
(assert-true test-name (= -3 (- 3 1 2 3)))
(assert-true test-name (= -3 (- 3)))

(define test-name "procedure /")
(assert-true test-name (= 2 (/ 4 2)))
(assert-true test-name (= 2 (/ 8 2 2)))
(assert-true test-name (= 1 (/ 54 2 9 3)))

(define test-name "procedure *")
(assert-true test-name (= 2 (* 1 2)))
(assert-true test-name (= -8 (* 4 -2)))
(assert-true test-name (= 40 (* 4 2 5)))

(total-report)



