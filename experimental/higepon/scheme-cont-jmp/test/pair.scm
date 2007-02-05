(assert-check-true "pair? should be #t"
                   (pair? (cons 1 2))
                   (pair? (list 1 2 3 4)))

(assert-check-false "pair? should be #f"
                    (pair? "string")
                    (pair? 1))

(assert-check-true "cons"
             (= 3 (car (cons 3 4)))
             (= 4 (cdr (cons 3 4)))
             (= 3 (cadr (cons (cons 1 2) (cons 3 4))))
             (= 2 (cdar (cons (cons 1 2) (cons 3 4))))
             (= 1 (caar (cons (cons 1 2) (cons 3 4))))
             (= 4 (cddr (cons (cons 1 2) (cons 3 4)))))

(let ((p (cons 1 2)))
  (set-car! p 3)
  (set-cdr! p 4)
  (assert-check-true "set-car!/set-cdr!"
                     (= (car p) 3)
                     (= (cdr p) 4)))

