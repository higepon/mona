(define find (lambda (pred list)
               (call/cc
                (lambda (return)
                  (for-each
                   (lambda (e) (if (pred e) (return e)))
                   list)
                  #f))))

(assert-check-true "call/cc"
                   (= (find even? (list 1 2 3 4)) 2)
                   (= (find odd? (list 1 2 3 4)) 1)
                   (= 5 (call-with-values (lambda () (values 4 5))
                          (lambda (a b) b)))
                   (= (call-with-values * -) -1)
                   )

;; (let ((total 0))
;;   (assert-check-true "dynamic-wind"
;;                      (= (dynamic-wind
;;                             (lambda () (set! total (+ total 1)))
;;                             (lambda () (set! total (+ total 1)) total)
;;                             (lambda () (set! total (+ total 2)))) 2)
;;                      (= total 4)
;; ))
