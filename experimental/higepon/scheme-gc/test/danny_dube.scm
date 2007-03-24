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
    (if (null? (car ll))
    '()
    (let ((tetes (map1 car ll))
          (queues (map1 cdr ll)))
      (cons (apply proc tetes)
        (apply map (cons proc queues)))))))

(define map1
  (lambda (f l)
    (if (null? l)
    l
    (cons (f (car l)) (map1 f (cdr l))))))

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

