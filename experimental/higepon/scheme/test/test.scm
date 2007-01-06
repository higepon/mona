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

(define test-name "if")
(assert-true test-name (if #t #t #f))
(assert-false test-name (if #t #f #t))
(assert-false test-name (if #f #t))
(define x 3)
(if #t (set! x 4) (set! x 5))
(assert-true test-name (= 4 x))
(define x 3)
(if #f (set! x 4) (set! x 5))
(assert-true test-name (= 5 x))

(define test-name "cons")
(assert-true test-name (= 3 (car (cons 3 4))))
(assert-true test-name (= 4 (cdr (cons 3 4))))
(assert-true test-name (= 3 (cadr (cons (cons 1 2) (cons 3 4)))))
(assert-true test-name (= 2 (cdar (cons (cons 1 2) (cons 3 4)))))
(assert-true test-name (= 1 (caar (cons (cons 1 2) (cons 3 4)))))
(assert-true test-name (= 4 (cddr (cons (cons 1 2) (cons 3 4)))))

(define test-name "number?")
(assert-true test-name (number? 3))
(assert-true test-name (number? (+ 1 2)))
(assert-false test-name (number? "string"))
(assert-false test-name (number? (cons 1 2)))


(define test-name "string?")
(assert-false test-name (string? 3))
(assert-false test-name (string? (+ 1 2)))
(assert-true test-name (string? "string"))
(assert-false test-name (string? (cons 1 2)))

(define test-name "string->number")
(assert-true test-name (= 1234 (string->number "1234")))

(define test-name "and")
(assert-true test-name (and 3))
(assert-true test-name (and #t))
(assert-true test-name (and #t #t))
(assert-true test-name (and #t #t #t))
(assert-true test-name (and 1 3 2))
(assert-true test-name (= 2 (and 1 3 2)))
(assert-true test-name (= 4 (and 1 3 2 4)))
(assert-false test-name (and #f))
(assert-false test-name (and #t #f))
(assert-false test-name (and #t #f #t))
(assert-false test-name (and 1 #f 2))
(assert-false test-name (= 2 (and 1 #f 2)))
(assert-false test-name (= 4 (and 1 #f 2 4)))

(define test-name "or")
(assert-true test-name (or 3))
(assert-true test-name (or #t))
(assert-true test-name (or #t #t))
(assert-true test-name (or #t #t #t))
(assert-true test-name (or 1 #f 2))
(assert-true test-name (or 1 3 2))
(assert-true test-name (= 2 (or 2 3 1)))
(assert-true test-name (= 4 (or 4 3 2 4)))
(assert-true test-name (= 2 (or #f 2 #f)))
(assert-true test-name (= 4 (or #f #f 4)))
(assert-false test-name (or #f))
(assert-false test-name (or #f #f))
(assert-false test-name (or #f #f #f))

(define test-name "begin")
(define x 0)
(assert-true test-name (= 4 (begin (+ 3 1))))
(assert-true test-name (= 4 (begin (set! x 1) 4)))
(assert-true test-name (= x 1))

(define test-name "lambda")
(assert-true test-name (= 3 ((lambda (x) (+ x 1)) 2)))
(assert-true test-name (= 3 ((lambda (x y) (+ x 1 y)) 1 1)))
(define x 0)
((lambda (x) (set! x 3)) 2)
;; todo
;; (assert-true test-name (= 0 x))

(define test-name "let")
(let ((a 3) (b 2))
  (assert-true test-name (= 5 (+ a b))))
(let ((a 3))
  (let ((b 2))
    (assert-true test-name (= 5 (+ a b)))))

;; todo
;; (define test-name "let*")
;; (let* ((a 3) (b 2))
;;   (assert-true test-name (= 5 (+ a b))))
;; (let* ((a 3))
;;   (let* ((b 2))
;;     (assert-true test-name (= 5 (+ a b)))))


(define test-name "cond")
(cond (#t (assert-true test-name #t))
      (else (assert-fail test-name "not reached")))
(cond (#f (assert-fail test-name "not reached"))
      (else (assert-true test-name #t)))
(cond (#f (assert-fail test-name "not reached"))
      (#t (assert-true test-name #t))
      (else (assert-fail test-name "not reached")))
(cond (#f (assert-fail test-name "not reached"))
      ((+ 1 2) (assert-true test-name #t))
      (else (assert-fail test-name "not reached")))
(cond (#f (assert-fail test-name "not reached"))
      (#t (assert-true test-name #t))
      (#f (assert-fail test-name "not reached")))
;(assert-true test-name (= 3 (cond (#t 3))))
;;(assert-true test-name (= 4 (cond (#f 3) (#t 4) (#f 2))))
;; (define-syntax cond*
;; (syntax-rules (else =>)
;; ((cond* (else result1 result2 ...))
;; (begin result1 result2 ...))
;; ((cond* (test => result))
;; (let ((temp test))
;; (if temp (result temp))))
;; ((cond* (test => result) clause1 clause2 ...)
;; (let ((temp test))
;; (if temp
;; (result temp)
;; (cond* clause1 clause2 ...))))
;; ((cond* (test)) test)))


;; ...をリネームするコードを書かないとダメだね．
;;(define-syntax cond*
;;  (syntax-rules (else =>)
;;     ((cond (else result1 result2 ...))
;;      (begin result1 result2 ...))
;;     ((cond (test => result))
;;      (let ((temp test))
;;        (if temp (result temp))))
;;     ((cond (test => result) clause1 clause2 ...)
;;      (let ((temp test))
;;        (if temp
;;            (result temp)
;;            (cond clause1 clause2 ...))))
;;     ((cond (test)) test)
;;     ((cond (test) clause1 clause2 ...)
;;      (let ((temp test))
;;        (if temp
;;            temp
;;            (cond clause1 clause2 ...))))
;;     ((cond (test result1 result2 ...))
;;      (if test (begin result1 result2 ...)))
;;     ((cond (test result1 result2 ...)
;;            clause1 clause2 ...)
;;      (if test
;;          (begin result1 result2 ...)
;;          (cond* clause1 clause2 ...)))))

;(assert-true test-name (= 3 ((lambda ()  (cond* (1 => (lambda (x) (+ x 2))) (else 8))))))
;; todo
;; (cond (#f (assert-fail test-name "not reached"))
;;       (#f (assert-fail test-name "not reached"))
;;       (#t (assert-true test-name #t)))

;; repor*t
(total-report)



