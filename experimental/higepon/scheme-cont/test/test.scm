(load "./test/scheme.scm")
(load "./test/unittest.scm")

(assert-check-true "if"
                   (if #t #t #f))

(assert-check-false "if should be #f"
                    (if #t #f #t)
                    (if #f #t))

(define test-name "set!")
(define x 3)
(if #t (set! x 4) (set! x 5))
(assert-true test-name (= 4 x))
(define x 3)
(if #f (set! x 4) (set! x 5))

(define xx 0)
(assert-check-true test-name
             (= ((lambda (y) (set! y 3) y) 1) 3)
             (= 5 x)
             (= 4 ((lambda (y) (set! y (+ y 2)) y) 2))      ; y has one scope.
             (= 4 ((lambda (xx) (set! xx (+ xx 2)) xx) 2)))  ; x has two scopes.

(assert-check-true "cons"
             (= 3 (car (cons 3 4)))
             (= 4 (cdr (cons 3 4)))
             (= 3 (cadr (cons (cons 1 2) (cons 3 4))))
             (= 2 (cdar (cons (cons 1 2) (cons 3 4))))
             (= 1 (caar (cons (cons 1 2) (cons 3 4))))
             (= 4 (cddr (cons (cons 1 2) (cons 3 4)))))

(assert-check-false "string?"
                    (string? 3)
                    (string? (cons 1 2))
                    (string? (+ 1 2)))
(assert-check-true "string?" (string? "string"))

(assert-check-true "string->number" (= 1234 (string->number "1234")))
(assert-check-true "string-append" (string=? (string-append "12" "34") "1234"))

(assert-check-true "and"
                   (and 3)
                   (and #t #t)
                   (and #t #t #t)
                   (and 1 3 2)
                   (= 2 (and 1 3 2))
                   (= 4 (and 1 3 2 4)))

(assert-check-false "and should be #f"
                    (and #f)
                    (and #t #f)
                    (and #t #f #t)
                    (and 1 #f 2)
                    (= 2 (and 1 #f 2))
                    (= 4 (and 1 #f 2 4)))

(assert-check-true "or"
                   (or 3)
                   (or #t)
                   (or #t #t)
                   (or #t #t #t)
                   (or 1 #f 2)
                   (or 1 3 2)
                   (= 2 (or 2 3 1))
                   (= 4 (or 4 3 2 4))
                   (= 2 (or #f 2 #f))
                   (= 4 (or #f #f 4)))

(assert-check-false "or should be #f"
                    (or #f)
                    (or #f #f)
                    (or #f #f #f))

(define x 0)
(assert-check-true "begin"
                   (= 4 (begin (+ 3 1)))
                   (= 4 (begin (set! x 1) 4))
                   (= x 1))

(assert-check-true "lambda"
                   (= 3 ((lambda (x) (+ x 1)) 2))
                   (= 3 ((lambda (x y) (+ x 1 y)) 1 1)))
(define x 0)
((lambda (x) (set! x 3)) 2)
;; todo
;; (assert-true test-name (= 0 x))

(let ((a 3) (b 2))
  (assert-true "let" (= 5 (+ a b))))
(let ((a 3))
  (let ((b 2))
    (assert-true "let" (= 5 (+ a b)))))

(define test-name "let*")
(let* ((a 3) (b 2))
  (assert-true "let*" (= 5 (+ a b))))
(let* ((a 3))
  (let* ((b 2))
    (assert-true "let*" (= 5 (+ a b)))))

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

(assert-check-true "cond"
                   (= 3 (cond (#t 3)))
                   (= 4 (cond (#f 3) (#t 4) (#f 2))))

(define fact
  (lambda (n)
    (if (= n 0)
        1
        (* n (fact (- n 1))))))
(assert-true "fact" (= 120 (fact 5)))

;; (define test-name "quote")
;; (display (car (quote ((x1 x2) ((y1 y2))))))
;; (define xyz 1234)
;; (assert-true test-name (= 1234 (eval (quote xyz))))

;; (define test-name "eval")
;; (eval (quote (define hoge+ (lambda (x) (+ 4 x)))))
;; (assert-true test-name (= 4 (hoge+ 0)))
;; (eval (quote (define-syntax pqr
;;                (syntax-rules ()
;;                  ((_ a) (+ a 3))))))
;; (assert-true test-name (= 7 (pqr 4)))


;; ...をリネームするコードを書かないとダメだね．
;; (define-syntax cond*
;;   (syntax-rules (else =>)
;;     ((cond* (else result1 result2 ...))
;;      (begin result1 result2 ...))
;;     ((cond* (test => result))
;;      (let ((temp test))
;;        (if temp (result temp))))
;;     ((cond* (test => result) clause1 clause2 ...)
;;      (let ((temp test))
;;        (if temp
;;            (result temp)
;;            (cond* clause1 clause2 ...))))
;;    ((cond* (test)) test)
;;     ((cond* (test) clause1 clause2 ...)
;;      (let ((temp test))
;;        (if temp
;;            temp
;;            (cond* clause1 clause2 ...))))
;;     ((cond* (test result1 result2 ...))
;;      (if test (begin result1 result2 ...)))
;;     ((cond* (test result1 result2 ...)
;;             clause1 clause2 ...)
;;      (if test
;;          (begin result1 result2 ...)
;;          (cond* clause1 clause2 ...)))))

;(cond* (3 (display 3)))
;;(cond* (#f 2) (else 5))

;; (define test-name "cond*")
;; (cond* (#t (assert-true test-name #t))
;;      (else (assert-fail test-name "not reached")))
;; (cond* (#f (assert-fail test-name "not reached"))
;;       (else (assert-true test-name #t)))
;; (cond* (#f (assert-fail test-name "not reached"))
;;       (#t (assert-true test-name #t))
;;       (else (assert-fail test-name "not reached")))
;; (cond* (#f (assert-fail test-name "not reached"))
;;       ((+ 1 2) (assert-true test-name #t))
;;       (else (assert-fail test-name "not reached")))
;; (cond* (#f (assert-fail test-name "not reached"))
;;       (#t (assert-true test-name #t))
;;       (#f (assert-fail test-name "not reached")))

;; (define-syntax c
;;   (syntax-rules ()
;;     ((c (test result1 result2 ...)
;;         clause1 clause2 ...)
;;      (if test
;;          (begin result1 result2 ...)
;;          (c clause1 clause2 ...)))))

;(assert-true test-name (= 3 ((lambda ()  (cond* (1 => (lambda (x) (+ x 2))) (else 8))))))
;; todo
;;  (cond (#f (assert-fail test-name "not reached"))
;;        (#f (assert-fail test-name "not reached"))
;;        (#t (assert-true test-name #t)))


;; todo
;; マクロが展開されておかしくなる！
;(display (car (quote (list 1 2 3 4))))

(define test-string1 "hige")
(define test-string2 test-string1)
(define test-pair1 (cons 1 2))
(define test-pair2 test-pair1)
(assert-check-true "eqv? - should be #t"
                   (eqv? 'a 'a)
                   (eqv? #\a #\a)
                   (eqv? #t #t)
                   (eqv? #f #f)
                   (eqv? 3 3)
                   (eqv? (quote a) (quote a))
                   (eqv? (quote ()) (quote ()))
                   (eqv? test-string1 test-string2)
                   (eqv? test-pair1 test-pair2))

(define p (lambda (x) (+ x 1)))
(assert-check-false "eqv? - should be #f different type"
                    (eqv? 'a 'b)
                    (eqv? #\a #\b)
                    (eqv? #t 2)
                    (eqv? #t "false")
                    (eqv? #t (quote (a b)))
                    (eqv? #t (cons 1 2))
                    (eqv? 2 "number")
                    (eqv? 2 (quote (a b)))
                    (eqv? 2 (cons 1 2))
                    (eqv? (quote (x y)) (cons 1 2))
                    (eqv? p p))

(assert-check-false "eqv? - should be #f"
                    (eqv? #t #f)
                    (eqv? #f #t)
                    (eqv? (quote ()) 3)
                    (eqv? (quote a) (quote b))
                    (eqv? "string" "string")
                    (eqv? (cons 1 2) (cons 1 2))
                    (eqv? 2 3)
                    (eqv? (lambda () 1) (lambda () 2)))

(assert-check-false "eqv? - R5RS doesn't say anything about these. So same as Gauche."
                    (eqv? "" "")
                    (eqv? (lambda (x) x)
                          (lambda (x) x))
                    (eqv? (lambda (x) x)
                          (lambda (y) y)))

(assert-check-true "quote"
                   (eqv? (quote a) (quote a))
                   (eqv? (car (quote (x y z))) (quote x))
                   (eqv? (cdr (quote (x y z))) (quote (y z))))

(assert-check-true "quote '"
                   (eqv? (caar (cons (cons 3 4) (cons 5 6))) 3)
                   (eqv? 'a 'a)
                   (eqv? (car '(x y z)) 'x)
                   (eqv? (cdr '(x y z)) '(y z)))

(assert-check-true "quote self evaluate"
                   '#t
                   (= '12345 12345)
                   (string=? '"abc" "abc"))

(assert-check-true "should be #t"
                   0
                   1
                   '()
                   null
                   'a
                   #t
                   (cons 1 2)
                   (lambda (x) x))

(assert-check-false "should be #f" #f)

(assert-check-true "eq?"
                   (eq? 'a 'a)
                   (eq? '() '())
                   (eq? car car)
                   (let ((x '(a)))
                     (eq? x x))
                   (let ((x '#()))
                     (eq? x x))
                   (let ((p (lambda (x) x)))
                     (eq? p p)))

(assert-check-false "eq? should be #f"
                    (eq? (list 'a) (list 'a)))

(load "./test/char.scm")
(load "./test/vector.scm")
(load "./test/symbol.scm")
(load "./test/number.scm")
(load "./test/continuation.scm")

;; report
(total-report)
