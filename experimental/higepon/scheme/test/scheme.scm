(define newline
  (lambda () (display "\n")))

;; for unittest.scm
(define write
  (lambda (o) (display o)))

(define zero?
  (lambda (a)
    (= 0 a)))

(define for-each
  (lambda (proc items)
    (if (null? items)
        "done"
        (begin (proc (car items))
               (for-each proc (cdr items))))))

;; fix me!
(define null 12345678)


(define null?
  (lambda (a)
    (= a null)))

(define-syntax list
  (syntax-rules ()
    ((_ a) (cons a null))
    ((_ a ...) (cons a (list ...)))))
;; fix me end!
