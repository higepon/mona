(load "./lib/unittest.scm")
;; (define-macro assert-check-true
;;   (lambda (test-name test)
;;     `(if (assert-true ,test-name ,test)
;;          #t
;;          (begin (display "test failed")
;;                 (write (quote ,test))))))

(define-macro assert-check-true
  (lambda (test-name . test)
    (print test)
    (if (null? test)
        #t
        `(if (assert-true ,test-name ,(car test))
             (assert-check-true ,test-name ,@(cdr test))
             (begin (display "test failed")
                    (write (quote ,(car test))))))))

(assert-check-true "hige-test"
                   (= 3 3)
                   (begin (display "second ")(= (+ 2 2) 4))
)

(define-macro bnd
  (lambda args
    (if (null? args)
	#t
	(if (null? (cdr args))
	    (car args)
	    `(if ,(car args)
		 (bnd ,@(cdr args))
		 #f)))))

(bnd (begin (display "first") #f) (display "second"))

;; (define-macro assert-check-true
;;   (lambda (test-name . args)
;;     (if `(assert-true ,test-name ,test)
                     
;;    `(assert-true ,test-name ,test)))


;; (assert-check-true "test ok" (begin (display "test done") (= 4 3)))
;; (total-report)

;; (define-macro macro-test (lambda (a) (write (pair? a)) `(display (quote ,a))))
;; (macro-test (+ 1 2))


