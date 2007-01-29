(define make-counter (lambda (init incremental)
                       (let ((x init)
                             (y incremental))
                         (lambda () (set! x (+ x y))))))

(define c1 (make-counter 0 1))
(define c2 (make-counter 0 2))

(assert-check-true "closure: 1"
                   (= (c1) 1)
                   (= (c2) 2)
                   (= (c1) 2)
                   (= (c2) 4)
                   (= (c1) 3)
                   (= (c2) 6))
