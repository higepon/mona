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

(assert-check-true "list? should be #f"
                   (list? (list 2 3))
                   (list? (cons 1 null)))

(assert-check-false "list? should be #f"
                   (list? "hige")
                   (list? 2)
                   (list? (cons 1 2))
                   (list? #\a))

(assert-check-true "length"
                   (= 3 (length (list 1 2 3)))
                   (= 0 (length null)))

(let* ((l (list 1 2 3 4)) (l2 (list-tail l 2)))
  (assert-check-true "list-ref"
                     (= 1 (list-ref l 0))
                     (= 2 (list-ref l 1))
                     (= 3 (list-ref l 2))
                     (= 3 (list-ref l2 0))
                     (= 4 (list-ref l2 1))))

(let ((l (reverse (list 1 2 3))))
  (assert-check-true "reverse"
                     (= 3 (list-ref l 0))
                     (= 2 (list-ref l 1))
                     (= 1 (list-ref l 2))))

(let* ((l (list 'a 'b 'c))
       (l2 (memq 'a l))
       (l3 (memq 'b l)))
  (assert-check-true "memq"
                     (eqv? (list-ref l2 0) 'a)
                     (eqv? (list-ref l2 1) 'b)
                     (eqv? (list-ref l3 0) 'b)
                     (eqv? (list-ref l3 1) 'c)))

(let* ((l (list 'a 'b 'c))
       (l2 (memq 'Z l)))
  (assert-check-false "memq should be #f"
                     l2))

(let* ((l (list 3 4 5))
       (l2 (memv 3 l))
       (l3 (memv 4 l)))
  (assert-check-true "memv"
                     (eqv? (list-ref l2 0) 3)
                     (eqv? (list-ref l2 1) 4)
                     (eqv? (list-ref l3 0) 4)
                     (eqv? (list-ref l3 1) 5)))

(let* ((l (list 1 2 3))
       (l2 (memq 'a l)))
  (assert-check-false "memv should be #f"
                     l2))

(let* ((e '((a 1) (b 2) (c 3)))
      (a (assq 'a e))
      (b (assq 'b e))
      (c (assq 'd e))
      (d (assv 5 '((2 3) (5 7) (11 13)))))
  (assert-check-true "assq"
                     (eqv? (list-ref a 0) 'a)
                     (eqv? (list-ref a 1) (string->symbol "1"))
;; todo
;;                     (eqv? (list-ref d 0) (string->symbol "5"))
;;                     (eqv? (list-ref d 1) (string->symbol "7"))
                     (eqv? (list-ref b 0) 'b)
                     (eqv? (list-ref b 1) (string->symbol "2")))
  (assert-check-false "assq #f"
                     (assq 'd e)
                     (assq (list 'a) '(((a)) ((b)) ((c))))))

(let* ((a (cons 2 3)))
  (assert-check-true "member"
                     (member (cons 2 3) (list a 'b))))

(assert-check-true "lambda's argument"
                   (null? ((lambda x x))))


;; (let* ((l (list 1 2)) (l2 (append l 3)))
;;   (assert-check-true "append"
;;                       (= 1 (list-ref l 0))))
;;                      (= 2 (list-ref l 1))
;;                      (= 3 (list-ref l 2))))
