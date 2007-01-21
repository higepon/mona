(assert-check-true "symbol?"
                   (symbol? 'foo)
                   (symbol? (car '(a b)))
                   (symbol? 'nil))

(assert-check-false "symbol? should be false"
                    (symbol? "bar")
;; todo
;;                    (symbol? '())
                    (symbol? #f))
(display (symbol->string 'xyz))

(assert-check-true "symbol->string"
                   (string=? "abc" (symbol->string 'abc)))



