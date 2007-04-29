;; (display (values 4 5))

;; ;; (display (call-with-values (lambda () (values 4 5))
;; ;;            (lambda (a b) b)))
;; (define map1
;;   (lambda (f l)
;; ;;     (display "map1")
;; ;;     (display l)
;; ;;     (newline)
;; ;;     (display "f")
;; ;;     (display f)
;; ;;     (newline)
;;     (if (null? l)
;;     l
;;     (cons (f (car l)) (map1 f (cdr l))))))

;; (define map
;;   (lambda (proc . ll)
;; ;;     (display "map")
;; ;;     (display ll)
;; ;;     (newline)
;; ;;     (display "map proc")
;; ;;     (display proc)
;; ;;     (newline)
;;     (if (null? (car ll))
;;     '()
;;     (let ((tetes (map1 car ll))
;;           (queues (map1 cdr ll)))
;;       (cons (apply proc tetes)
;;         (apply map (cons proc queues)))))))

;; (define null '())

;; (define null?
;;   (lambda (x)
;;     (eqv? x null)))

;; (define call/cc call-with-current-continuation)

;; (define zero?
;;   (lambda (x)
;;     (= 0 x)))

;; (define for-each
;;   (lambda (proc . ll)
;;     (if (null? (car ll))
;;         #f
;;     (let* ((tetes (map car ll))
;;            (queues (map cdr ll)))
;;       (apply proc tetes)
;;       (apply for-each (cons proc queues))))))

(define find (lambda (pred list)
               (call/cc
                (lambda (return)
                  (for-each
                   (lambda (e) (if (pred e) (return e)))
                   list)
                  #f))))

;; (display (find zero? '(1 3 5 0 6 7)))

(assert-check-true "call/cc"
                   (= (find even? (list 1 2 3 4)) 2)
                   (= (find odd? (list 1 2 3 4)) 1)
;;                    (= 5 (call-with-values (lambda () (values 4 5))
;;                           (lambda (a b) b)))
;;;                    (= (call-with-values * -) -1)
                  )

;; ;; ;; (let ((total 0))
;; ;; ;;   (assert-check-true "dynamic-wind"
;; ;; ;;                      (= (dynamic-wind
;; ;; ;;                             (lambda () (set! total (+ total 1)))
;; ;; ;;                             (lambda () (set! total (+ total 1)) total)
;; ;; ;;                             (lambda () (set! total (+ total 2)))) 2)
;; ;; ;;                      (= total 4)
;; ;; ;; ))
