(define f (lambda (a . b)
            (display a)
            (display b)))

(define caar (lambda (p) (car (car p))))
(define cadr (lambda (p) (car (cdr p))))
(define cdar (lambda (p) (cdr (car p))))
(define cddr (lambda (p) (cdr (cdr p))))
(define caaar (lambda (p) (caar (car p))))
(define caadr (lambda (p) (caar (cdr p))))
(define cadar (lambda (p) (cadr (car p))))
(define caddr (lambda (p) (cadr (cdr p))))
(define cdaar (lambda (p) (cdar (car p))))
(define cdadr (lambda (p) (cdar (cdr p))))
(define cddar (lambda (p) (cddr (car p))))
(define cdddr (lambda (p) (cddr (cdr p))))
(define caaaar (lambda (p) (caaar (car p))))
(define caaadr (lambda (p) (caaar (cdr p))))
(define caadar (lambda (p) (caadr (car p))))
(define caaddr (lambda (p) (caadr (cdr p))))
(define cadaar (lambda (p) (cadar (car p))))
(define cadadr (lambda (p) (cadar (cdr p))))
(define caddar (lambda (p) (caddr (car p))))
(define cadddr (lambda (p) (caddr (cdr p))))
(define cdaaar (lambda (p) (cdaar (car p))))
(define cdaadr (lambda (p) (cdaar (cdr p))))
(define cdadar (lambda (p) (cdadr (car p))))
(define cdaddr (lambda (p) (cdadr (cdr p))))
(define cddaar (lambda (p) (cddar (car p))))
(define cddadr (lambda (p) (cddar (cdr p))))
(define cdddar (lambda (p) (cdddr (car p))))
(define cddddr (lambda (p) (cdddr (cdr p))))

(define call/cc call-with-current-continuation)

(define error (lambda x
                (display "error:")
                (for-each (lambda (s) (display s) (display " "))  x)))

(define zero?
  (lambda (x)
    (= 0 x)))

(define even?
  (lambda (x)
    (let ((quotient (/ x 2)))
      (= (- (* quotient 2) x) 0))))

(define odd?
  (lambda (x)
    (not (even? x))))

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
    ((_ a b ...) (cons a (list b ...)))))

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

(define-syntax case
  (syntax-rules (else)
    ((case (key ...)
       clauses ...)
     (let ((atom-key (key ...)))
       (case atom-key clauses ...)))
    ((case key
       (else result1 result2 ...))
     (begin result1 result2 ...))
    ((case key
       ((atoms ...) result1 result2 ...))
     (if (memv key (list atoms ...))
         (begin result1 result2 ...)))
    ((case key
       ((atoms ...) result1 result2 ...)
       clause clauses ...)
     (if (memv key (list atoms ...))
         (begin result1 result2 ...)
         (case key clause clauses ...)))))

(define-syntax cond
  (syntax-rules (else =>)
    ((cond (else result1 result2 ...))
     (begin result1 result2 ...))
    ((cond (test => result))
     (let ((temp test))
       (if temp (result temp))))
    ((cond (test => result) clause1 clause2 ...)
     (let ((temp test))
       (if temp
           (result temp)
           (cond clause1 clause2 ...))))
    ((cond (test)) test)
    ((cond (test) clause1 clause2 ...)
     (let ((temp test))
       (if temp
           temp
           (cond clause1 clause2 ...))))
    ((cond (test result1 result2 ...))
     (if test (begin result1 result2 ...)))
    ((cond (test result1 result2 ...)
           clause1 clause2 ...)
     (if test
         (begin result1 result2 ...)
         (cond clause1 clause2 ...)))))

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

(define make-promise
  (lambda (proc)
    (let ((result-ready? #f)
          (result #f))
      (lambda ()
        (if result-ready?
            result
            (let ((x (proc)))
              (if result-ready?
                  result
                  (begin (set! result-ready? #t)
                         (set! result x)
                         result))))))))

(define-syntax delay
  (syntax-rules ()
    ((delay expression)
     (make-promise (lambda () expression)))))

(define force
  (lambda (object)
    (object)))

(define values (lambda things
                 (call-with-current-continuation
                  (lambda (cont) (apply cont things)))))

(define-syntax aif
  (syntax-rules ()
    ((_ it a b c ...)
     (let ((it a)) (if it b c ...)))))

(define-syntax awhen
  (syntax-rules ()
    ((_ it pred body ...)
     (aif it pred (begin body ...)))))

(define-syntax awhile
  (syntax-rules ()
    ((_ it pred a)
     (do ((it pred pred))
         ((not it))
       a))))

(define-syntax aand
  (syntax-rules ()
    ((_ it a) #t
     (_ it a b c ...) (aif it a (_ it b c ...) #f))))

     
;; 		      (do ((expr (read port) (read port))
;; 			   (lst '() (cons expr lst)))
;; 			  ((eof-object? expr)
;; 			   (apply append lst))))))))

(define-syntax do
  (syntax-rules ()
    ;; 変数2つ commandなし
    ((_ ((a b c) (d e f))
        (pred g ...))
     (let loop ((a b) (d e))
       (if pred (begin g ...)
           (begin (loop c f)))))
    ;; 変数2つ commandあり
    ((_ ((a b c) (d e f))
        (pred g ...)
        x ...)
     (let loop ((a b) (d e))
       (if pred (begin g ...)
           (begin x ... (loop c f)))))
    ;; 変数2つ commandあり
    ((_ ((a b c) (d e))
        (pred)
        x ...)
     (let loop ((a b) (d e))
       (if pred #t
           (begin x ... (loop c e)))))
    ;; 変数1つ pred なし
    ((_ ((a b c))
        (pred)
       command ...)
     (let loop ((a b))
       (if pred #t
           (begin command ... (loop c)))))
    ;; 変数1つ commandあり
    ((_ ((a b c))
        (pred d ...)
       command ...)
     (let loop ((a b))
       (if pred (begin d ...)
           (begin command ... (loop c)))))))

;; for test
;; (define-syntax unquote
;;   (syntax-rules ()
;;     ((_ a) 
;;      (eval a (scheme-report-environment 5)))))

;(load "lib/DANNY.SCM")
;(load "slib/Template.scm")
;(slib:report)

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

(define member
  (lambda (obj list)
    (generic-member equal? obj list)))

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
(define assoc (lambda (obj alist) (generic-assoc equal? obj alist)))

(define char-ci=?
  (lambda (c1 c2) (char=? (char-downcase c1) (char-downcase c2))))
(define char-ci<?
  (lambda (c1 c2) (char<? (char-downcase c1) (char-downcase c2))))
(define char-ci>?
  (lambda (c1 c2) (char>? (char-downcase c1) (char-downcase c2))))
(define char-ci<=?
  (lambda (c1 c2) (char<=? (char-downcase c1) (char-downcase c2))))
(define char-ci>=?
  (lambda (c1 c2) (char>=? (char-downcase c1) (char-downcase c2))))

(define char-upper-case? (lambda (c) (and (char<=? #\A c) (char<=? c #\Z))))
(define char-lower-case? (lambda (c) (and (char<=? #\a c) (char<=? c #\z))))

(define char-upcase
  (lambda (c)
    (if (char-lower-case? c)
        (integer->char (+ (char->integer c)
                          (- (char->integer #\A) (char->integer #\a))))
        c)))

(define char-downcase
  (lambda (c)
    (if (char-upper-case? c)
        (integer->char (+ (char->integer c)
                          (- (char->integer #\a) (char->integer #\A))))
        c)))

(define char-alphabetic?
  (lambda (c) (and (char-ci<=? #\a c) (char-ci<=? c #\z))))
(define char-numeric? (lambda (c) (and (char<=? #\0 c) (char<=? c #\9))))
(define char-whitespace?
  (lambda (c)
    (or (char=? c #\space)
    (char=? c (integer->char 9))     ; Tab
    (char=? c #\newline)
    (char=? c (integer->char 12))    ; FF
    (char=? c (integer->char 13))))) ; CR
(define char-upper-case? (lambda (c) (and (char<=? #\A c) (char<=? c #\Z))))
(define char-lower-case? (lambda (c) (and (char<=? #\a c) (char<=? c #\z))))

(define char-upcase
  (lambda (c)
    (if (char-lower-case? c)
    (integer->char (+ (char->integer c)
              (- (char->integer #\A) (char->integer #\a))))
    c)))
(define char-downcase
  (lambda (c)
    (if (char-upper-case? c)
    (integer->char (+ (char->integer c)
              (- (char->integer #\a) (char->integer #\A))))
    c)))

(define string-compare
  (lambda (rel<? rel=? s1 s2)
    (let* ((len1 (string-length s1))
       (len2 (string-length s2))
       (len (min len1 len2)))
      (let loop ((pos 0))
    (if (< pos len)
        (let* ((c1 (string-ref s1 pos))
           (c2 (string-ref s2 pos)))
          (cond ((rel<? c1 c2)
             -1)
            ((rel=? c1 c2)
             (loop (+ pos 1)))
            (else
             1)))
        (cond ((< len1 len2)
           -1)
          ((= len1 len2)
           0)
          (else
           1)))))))

(define string<?
  (lambda (s1 s2) (<  (string-compare char<? char=? s1 s2) 0)))
(define string>?
  (lambda (s1 s2) (>  (string-compare char<? char=? s1 s2) 0)))
(define string<=?
  (lambda (s1 s2) (<= (string-compare char<? char=? s1 s2) 0)))
(define string>=?
  (lambda (s1 s2) (>= (string-compare char<? char=? s1 s2) 0)))
(define string-ci=?
  (lambda (s1 s2) (=  (string-compare char-ci<? char-ci=? s1 s2) 0)))
(define string-ci<?
  (lambda (s1 s2) (<  (string-compare char-ci<? char-ci=? s1 s2) 0)))
(define string-ci>?
  (lambda (s1 s2) (>  (string-compare char-ci<? char-ci=? s1 s2) 0)))
(define string-ci<=?
  (lambda (s1 s2) (<= (string-compare char-ci<? char-ci=? s1 s2) 0)))
(define string-ci>=?
  (lambda (s1 s2) (>= (string-compare char-ci<? char-ci=? s1 s2) 0)))

(define substring
  (lambda (str start end)
    (let* ((len (- end start))
       (newstr (make-string len)))
      (let loop ((pos 0))
    (if (< pos len)
        (begin
          (string-set! newstr pos (string-ref str (+ start pos)))
          (loop (+ pos 1)))))
      newstr)))

(define string-append
  (lambda ls
    (let* ((llen (map string-length ls))
       (totlen (foldl + 0 llen))
       (newstring (make-string totlen))
       (iter (lambda (iter ls llen from to)
           (if (< to totlen)
               (if (< from (car llen))
               (begin
                 (string-set! newstring
                      to
                      (string-ref (car ls) from))
                 (iter iter ls llen (+ from 1) (+ to 1)))
               (iter iter (cdr ls) (cdr llen) 0 to))))))
      (iter iter ls llen 0 0)
      newstring)))

(define map
  (lambda (proc . ll)
;;     (display "map")
;;     (display ll)
;;     (newline)
;;     (display "map proc")
;;     (display proc)
;;     (newline)
    (if (null? (car ll))
    '()
    (let ((tetes (map1 car ll))
          (queues (map1 cdr ll)))
      (cons (apply proc tetes)
        (apply map (cons proc queues)))))))

(define map1
  (lambda (f l)
;;     (display "map1")
;;     (display l)
;;     (newline)
;;     (display "f")
;;     (display f)
;;     (newline)
    (if (null? l)
    l
    (cons (f (car l)) (map1 f (cdr l))))))

(define f (lambda (a . b)
            (display a)
            (display b)))

(define foldl
  (lambda (binop start l)
    (if (null? l)
    start
    (foldl binop (binop start (car l)) (cdr l)))))

(define string->list
  (lambda (str)
    (let loop ((pos (- (string-length str) 1)) (l '()))
      (if (< pos 0)
      l
      (loop (- pos 1) (cons (string-ref str pos) l))))))

(define list->string
  (lambda (l)
    (let* ((len (length l))
       (newstring (make-string len))
       (iter (lambda (iter l to)
           (if (< to len)
               (begin
             (string-set! newstring to (car l))
             (iter iter (cdr l) (+ to 1)))))))
      (iter iter l 0)
      newstring)))

(define string-fill!
  (lambda (str fill)
    (let loop ((pos (- (string-length str) 1)))
      (if (>= pos 0)
      (begin
        (string-set! str pos fill)
        (loop (- pos 1)))))))

(define vector->list
  (lambda (v)
    (let loop ((pos (- (vector-length v) 1)) (l '()))
      (if (< pos 0)
      l
      (loop (- pos 1) (cons (vector-ref v pos) l))))))
(define list->vector
  (lambda (l)
    (let* ((len (length l))
       (v (make-vector len)))
      (let loop ((l l) (pos 0))
    (if (not (null? l))
        (begin
          (vector-set! v pos (car l))
          (loop (cdr l) (+ pos 1)))))
      v)))
(define vector-fill!
  (lambda (v fill)
    (let loop ((pos (- (vector-length v) 1)))
      (if (>= pos 0)
      (begin
        (vector-set! v pos fill)
        (loop (- pos 1)))))))

(define for-each
  (lambda (proc . ll)
;;     (display "for-each")
;;     (display ll)
;;     (newline)
;;     (display "for-each proc")
;;     (display proc)
;;     (newline)
    (if (null? (car ll))
        #f
    (let* ((tetes (map car ll))
           (queues (map cdr ll)))
      (apply proc tetes)
      (apply for-each (cons proc queues))))))

(define equal?
  (lambda (d1 d2)
    (cond ((eqv? d1 d2)
       #t)
      ((and (pair? d1) (pair? d2))
       (and (equal? (car d1) (car d2)) (equal? (cdr d1) (cdr d2))))
      ((and (vector? d1) (vector? d2))
       (let ((len (vector-length d1)))
         (if (not (= len (vector-length d2)))
         #f
         (let loop ((pos 0))
           (cond ((>= pos len)
              #t)
             ((equal? (vector-ref d1 pos) (vector-ref d2 pos))
              (loop (+ pos 1)))
             (else
              #f))))))
      ((and (string? d1) (string? d2))
       (string=? d1 d2))
      (else
       #f))))

(define mathgcd2
  (lambda (n1 n2)
    (let loop ((n1 (abs n1)) (n2 (abs n2)))
      (cond ((zero? n1) n2)
        ((zero? n2) n1)
        (else
         (let ((grand (max n1 n2))
           (petit (min n1 n2)))
           (loop petit (modulo grand petit))))))))

(define mathlcm2
  (lambda (n1 n2)
    (cond ((zero? n1) (abs n2))
      ((zero? n2) (abs n1))
      (else
       (let ((n1 (abs n1))
         (n2 (abs n2)))
         (/ (* n1 n2) (mathgcd2 n1 n2)))))))

(define gcd (lambda l (foldl mathgcd2 0 l)))
(define lcm (lambda l (foldl mathlcm2 1 l)))

(define rationalize (lambda (x y) x))

(define string (lambda l (list->string l)))

(define append2
  (lambda (l1 l2)
    (if (null? l1)
    l2
    (let ((tete (cons (car l1) l2)))
      (let loop ((cur tete) (l1 (cdr l1)))
        (if (null? l1)
        tete
        (begin
          (set-cdr! cur (cons (car l1) l2))
          (loop (cdr cur) (cdr l1)))))))))

(define append
  (lambda ll
    (foldr1 append2 (cons '() ll))))

(define foldr1
  (lambda (binop l)
    (if (null? (cdr l))
	(car l)
	(binop (car l) (foldr1 binop (cdr l))))))


; (load "/SERVERS/MONA.SCM")
; (load "/SERVERS/DANNY.SCM")
(display "*+********************************")
(for-each display '(a b c))
(for-each display (ls))
