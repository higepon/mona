(assert-check-true "symbol?"
                   (symbol? 'foo)
                   (symbol? (car '(a b)))
                   (symbol? 'nil))

(assert-check-false "symbol? should be false"
                    (symbol? "bar")
;; todo
;;                    (symbol? '())
                    (symbol? #f))
;;(display (symbol->string 'xyz))

(assert-check-true "symbol->string"
                   (string=? "abc" (symbol->string 'abc))
                   (string=? "abc" (symbol->string (string->symbol "abc"))))

(assert-check-true "double quote"
                   (string=? "a"(symbol->string (eval ''a (scheme-report-environment 5))))
)

(define a "higepon")
(define b '(1 2))
(define hoge (lambda (a) (* a 2)))
(assert-check-true "quasiquote"
                   (eq? 'a `a)
                   (string=? "higepon" `,a)
                   (eqv? 3 `3)
                   (equal? `(hoge ,(+ 1 2) 4) '(hoge 3 4))
                   (equal? (let ((name 'a)) `(hoge ,name ',name)) '(hoge a (quote a)))
                   (equal? `(a ,@b) '(a 1 2))
                   (equal? `(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b) '(a 3 4 5 6 b))
                   (equal? `(( foo ,(- 10 3))  ,(car '(cons))) '((foo 7) cons))
                   (equal? `#(10 5 ,(hoge 4) ,@(map hoge '(16 9)) 8) '#(10 5 8 32 18 8))
)
