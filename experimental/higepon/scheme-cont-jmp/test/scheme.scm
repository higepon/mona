(define caar
  (lambda (x)
    (car (car x))))

(define cdar
  (lambda (x)
    (cdr (car x))))

(define cadr
  (lambda (x)
    (car (cdr x))))

(define cddr
  (lambda (x)
    (cdr (cdr x))))

;; (define newline
;;   (lambda () (display "\n")))

;; ;; for unittest.scm
;; (define write
;;   (lambda (o) (display o)))

(define zero?
  (lambda (x)
    (= 0 x)))n

(define even?
  (lambda (x)
    (let ((quotient (/ x 2)))
      (= (- (* quotient 2) x) 0))))

(define odd?
  (lambda (x)
    (not (even? x))))

(define for-each
  (lambda (proc items)
    (if (null? items)
        "done"
        (begin (proc (car items))
               (for-each proc (cdr items))))))

(define not
  (lambda (condition)
    (if condition #f #t)))

(define null '())

(define null?
  (lambda (x)
    (eqv? x null)))

(define-syntax list
  (syntax-rules ()
    ((_ a) (cons a null))
    ((_ a ...) (cons a (list ...)))))

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

(define positive? (lambda (x) (> x 0)))
(define negative? (lambda (x) (< x 0)))
(define abs (lambda (x) (if (negative? x) (* -1 x) x)))
(define not (lambda (x) (if x #f #t)))

(define call-with-input-file (lambda (file proc)
                               (let* ((port (open-input-port file)) (ret (proc port)))
                               (close-input-port port)
                               ret)))

(define call-with-output-file (lambda (file proc)
                               (let* ((port (open-output-port file)) (ret (proc port)))
                               (close-output-port port)
                               ret)))

(define with-input-from-file (lambda (file proc)
                               (let* ((default-input-port (current-input-port)) (port (open-input-port file)))
                                 (set-current-input-port! port)
                                 (proc)
                                 (close-input-port port)
                                 (set-current-input-port! default-input-port))))

(define with-output-from-file (lambda (file proc)
                               (let* ((default-output-port (current-output-port)) (port (open-output-port file)))
                                 (set-current-output-port! port)
                                 (proc)
                                 (close-output-port port)
                                 (set-current-output-port! default-output-port))))


(define complex? (lambda (x) (number? x)))
(define real? (lambda (x) (number? x)))
(define rational? (lambda (x) (number? x)))
(define integer? (lambda (x) (number? x)))

(define exact? (lambda (x) (number? x)))
(define inexact? (lambda (x) #f))

(define quotient (lambda (n1 n2) (/ n1 n2)))

(define modulo (lambda (n1 n2)
                 (let* ((q (quotient n1 n2)) (v (- n1 (* q n2))))
                   (if (positive? n1) v (* -1 v)))))

(define reminder (lambda (n1 n2)
                   (let ((q (quotient n1 n2)))
                     (- n1 (* q n2)))))

(define numerator (lambda (x) (not-supported "numerator")))
(define denominator (lambda (x) (not-supported "denominator")))

(define exp  (lambda (x) (not-supported "exp ")))
(define log  (lambda (x) (not-supported "log ")))
(define sin  (lambda (x) (not-supported "sin ")))
(define cos  (lambda (x) (not-supported "cos ")))
(define tan  (lambda (x) (not-supported "tan ")))
(define asin (lambda (x) (not-supported "asin")))
(define acos (lambda (x) (not-supported "acos")))
(define atan (lambda (x) (not-supported "atan")))
(define sqrt (lambda (x) (not-supported "sqrt")))
(define expt (lambda (x y) (not-supported "expt")))
(define make-rectanglar (lambda (x y) (not-supported "make-rectanglar")))
(define make-polar (lambda (x y) (not-supported "make-polar")))
(define real-part (lambda (x) (not-supported "real-part")))
(define imag-part (lambda (x) (not-supported "imag-part")))
(define magnitude (lambda (z) (not-supported "magnitude")))
(define angle (lambda (z) (not-supported "angle")))
(define exact->inexact (lambda (z) (not-supported "exact->inexact")))
(define inexact->exact (lambda (z) (not-supported "inexact->exact")))

(define floor (lambda (x) x))
(define ceiling (lambda (x) x))
(define truncate (lambda (x) x))
(define round (lambda (x) x))

; Copyright (C) 1995 Danny Dube, Universite de Montreal. All rights reserved.

;
; Fonctions implantees dans le noyau. Pour savoir lesquelles
; sont visibles, voir sections plus bas
;

(define max (lambda l (foldl1 max2 l)))
(define min (lambda l (foldl1 min2 l)))


(define foldl1
  (lambda (binop l)
    (if (null? (cdr l))
    (car l)
    (foldl1 binop (cons (binop (car l) (cadr l))
                (cddr l))))))

(define max2 (lambda (x y) (if (> x y) x y)))
(define min2 (lambda (x y) (if (< x y) x y)))

(define list?
  (lambda (l)
    (cond ((null? l)
       #t)
      ((not (pair? l))
       #f)
      (else
       (let loop ((slow l) (fast (cdr l)) (phase 2))
         (cond ((null? fast)
            #t)
           ((not (pair? fast))
            #f)
           ((eq? slow fast)
            #f)
           ((= phase 1)
            (loop slow (cdr fast) 2))
           (else
            (loop (cdr slow) (cdr fast) 1))))))))

(define length
  (lambda (l)
    (let loop ((l l) (len 0))
      (if (null? l)
      len
      (loop (cdr l) (+ len 1))))))

;; 引数おかしい
;; (define append
;;   (lambda ll
;;     (foldr1 append2 (cons '() ll))))

;; (define foldr1
;;   (lambda (binop l)
;;     (if (null? (cdr l))
;;     (car l)
;;     (binop (car l) (foldr1 binop (cdr l))))))

;; (define append2
;;   (lambda (l1 l2)
;;     (if (null? l1)
;;     l2
;;     (let ((tete (cons (car l1) l2)))
;;       (let loop ((cur tete) (l1 (cdr l1)))
;;         (if (null? l1)
;;         tete
;;         (begin
;;           (set-cdr! cur (cons (car l1) l2))
;;           (loop (cdr cur) (cdr l1)))))))))

(define list-tail
  (lambda (l pos)
    (if (= pos 0)
    l
    (list-tail (cdr l) (- pos 1)))))
(define list-ref (lambda (l pos) (car (list-tail l pos))))

(define reverse
  (lambda (l)
    (let loop ((l l) (rl '()))
      (if (null? l)
      rl
      (loop (cdr l) (cons (car l) rl))))))

(define generic-member
  (lambda (releq obj list)
    (if (null? list)
    #f
    (if (releq (car list) obj)
        list
        (generic-member releq obj (cdr list))))))

(define memq
  (lambda (obj list)
    (generic-member eq? obj list)))

(define memv
  (lambda (obj list)
    (generic-member eqv? obj list)))

(define generic-assoc
  (lambda (releq obj alist)
    (cond ((null? alist)
       #f)
      ((releq (car (car alist)) obj)
       (car alist))
      (else
       (generic-assoc releq obj (cdr alist))))))

(define assq (lambda (obj alist) (generic-assoc eq? obj alist)))
(define assv (lambda (obj alist) (generic-assoc eqv? obj alist)))
