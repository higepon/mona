(let* ((l (map (lambda (x) (+ 1 x)) (list 1 2 3 4))))
  (assert-check-true "map"
                     (= 2 (list-ref l 0))
                     (= 3 (list-ref l 1))
                     (= 4 (list-ref l 2))
                     (= 5 (list-ref l 3))))

(let ((l (map + '(1 2 3) '(4 5 6))))
  (assert-check-true "map"
                     (= 5 (list-ref l 0))
                     (= 7 (list-ref l 1))
                     (= 9 (list-ref l 2))
))

(let ((v (make-vector 5)))
  (for-each (lambda (i)
              (vector-set! v i (* i i)))
            (list 0 1 2 3 4))
  (assert-check-true "for-each"
                     (= 5 (vector-length v))
                     (= 0 (vector-ref v 0))
                     (= 1 (vector-ref v 1))
                     (= 4 (vector-ref v 2))
                     (= 9 (vector-ref v 3))
                     (= 16 (vector-ref v 4))
))

(let ((v (make-vector 5)))
  (for-each (lambda (i j)
              (vector-set! v i (+ i j)))
            (list 0 1 2 3 4) (list 0 1 2 3 4))
  (assert-check-true "for-each"
                     (= 5 (vector-length v))
                     (= 0 (vector-ref v 0))
                     (= 2 (vector-ref v 1))
                     (= 4 (vector-ref v 2))
                     (= 6 (vector-ref v 3))
                     (= 8 (vector-ref v 4))
))

(let ((p (delay (+ 3 2))))
  (assert-check-true "delay / force"
                     (= (force p) 5)))

(let* ((count 123) (p (delay (begin (set! count (+ count 1)) count))))
  (assert-check-true "delay / force"
                     (= (force p) 124)
                     (= (force p) 124)
))

(assert-check-true "equal?"
                   (equal? 'a 'a)
                   (equal? '(a) '(a))
                   (equal? '(a (b) c) '(a (b) c))
                   (equal? "abc" "abc")
                   (equal? 2 2)
                   (equal? (make-vector 5 'a) (make-vector 5 'a)))

(assert-check-true "apply"
                   (= 13 (apply + 1 2 3 (list 3 4)))
)
