(define caar
  (lambda (x)
    (car (car x))))

(display (caar (cons (cons 3 4) (cons 5 6))))

(define cdar
  (lambda (x)
    (cdr (car x))))

(define cadr
  (lambda (x)
    (car (cdr x))))

(define cddr
  (lambda (x)
    (cdr (cdr x))))

(define newline
  (lambda () (display "\n")))

;; for unittest.scm
(define write
  (lambda (o) (display o)))

(define zero?
  (lambda (x)
    (= 0 x)))

(define for-each
  (lambda (proc items)
    (if (null? items)
        "done"
        (begin (proc (car items))
               (for-each proc (cdr items))))))

(define not
  (lambda (condition)
    (if condition #f #t)))

;; fix me!
(define null 12345678)


(define null?
  (lambda (x)
    (= x null)))

(define-syntax list
  (syntax-rules ()
    ((_ a) (cons a null))
    ((_ a ...) (cons a (list ...)))))
;; fix me end!

(define-syntax and
  (syntax-rules ()
    ((and) #t)
    ((and test) test)
    ((and test1 test2 ...)
     (if test1 (and test2 ...) #f))))

(define-syntax or
  (syntax-rules ()
    ((or) #f)
    ((or test) test)
    ((or test1 test2 ...)
     (let ((x test1))
       (if x x (or test2 ...))))))

