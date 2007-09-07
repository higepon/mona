(assert-check-true "vector?"
                   (vector? (vector 3 3 3 3)))

(assert-check-false "vector? should be #f"
                   (vector? #f)
                   (vector? 1)
                   (vector? #\a)
                   (vector? (cons 1 2))
                   (vector? 'a))

(let* ((length 10) (v (make-vector length)) (str "hoge"))
  (assert-check-true "vector-length" (= length (vector-length v)))
  (vector-set! v 0 'a)
  (vector-set! v 1 1)
  (vector-set! v 2 str)
  (assert-check-true "vector-ref"
                     (eqv? (vector-ref v 0) 'a)
                     (= (vector-ref v 1) 1)
                     (eqv? (vector-ref v 2) str))
)

(let* ((length 3) (str "hoge") (vv (make-vector length str)))
  (assert-check-true "vector-length"
                     (= length (vector-length vv))
                     (eqv? (vector-ref vv 0) str)
                     (eqv? (vector-ref vv 1) str)
                     (eqv? (vector-ref vv 2) str))
)

(let* ((length 4) (str "hoge") (vv (vector 1 #\2 '3 (quote 4))))
  (assert-check-true "vector-length"
                     (= length (vector-length vv))
                     (eqv? (vector-ref vv 0) 1)
                     (eqv? (vector-ref vv 1) #\2)
                     (eqv? (vector-ref vv 2) '3)
                     (eqv? (vector-ref vv 3) '4))
)

(let* ((length 4) (str "hoge") (vv '#(1 #\2 '3 (quote 4))))
  (assert-check-true "vector #("
                     (= length (vector-length vv))
                     (eqv? (vector-ref vv 0) 1)
                     (eqv? (vector-ref vv 1) #\2)
                     (equal? (vector-ref vv 2) ''3)
                     (equal? (vector-ref vv 3) ''4))
)

(let* ((v '#(1 2 3))
       (l (vector->list v)))
  (assert-check-true "vector->list"
                     (= 3 (length l))
                     (= (list-ref l 0) 1)
                     (= (list-ref l 1) 2)
                     (= (list-ref l 2) 3)))

(let* ((l (list 1 2 3))
       (v (list->vector l)))
  (assert-check-true "list->vector"
                     (= 3 (vector-length v))
                     (= (vector-ref v 0) 1)
                     (= (vector-ref v 1) 2)
                     (= (vector-ref v 2) 3)))

(let* ((v '#(1 2 3)))
  (vector-fill! v #\a)
  (assert-check-true "vector-fill!"
                     (= 3 (vector-length v))
                     (char=? (vector-ref v 0) #\a)
                     (char=? (vector-ref v 1) #\a)
                     (char=? (vector-ref v 2) #\a)))
