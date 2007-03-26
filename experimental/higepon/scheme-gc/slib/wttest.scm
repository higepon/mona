;;;  "wttrtst.scm" Test Weight balanced trees		-*-Scheme-*-
;;;  Copyright (c) 1993-1994 Stephen Adams
;;;
;;; Copyright (c) 1993-94 Massachusetts Institute of Technology
;;;
;;; This material was developed by the Scheme project at the
;;; Massachusetts Institute of Technology, Department of Electrical
;;; Engineering and Computer Science.  Permission to copy and modify
;;; this software, to redistribute either the original software or a
;;; modified version, and to use this software for any purpose is
;;; granted, subject to the following restrictions and understandings.
;;;
;;; 1. Any copy made of this software must include this copyright
;;; notice in full.
;;;
;;; 2. Users of this software agree to make their best efforts (a) to
;;; return to the MIT Scheme project any improvements or extensions
;;; that they make, so that these may be included in future releases;
;;; and (b) to inform MIT of noteworthy uses of this software.
;;;
;;; 3. All materials developed as a consequence of the use of this
;;; software shall duly acknowledge such use, in accordance with the
;;; usual standards of acknowledging credit in academic research.
;;;
;;; 4. MIT has made no warranty or representation that the operation
;;; of this software will be error-free, and MIT is under no
;;; obligation to provide any services, by way of maintenance, update,
;;; or otherwise.
;;;
;;; 5. In conjunction with products arising from the use of this
;;; material, there shall be no use of the name of the Massachusetts
;;; Institute of Technology nor of any adaptation thereof in any
;;; advertising, promotional, or sales literature without prior
;;; written consent from MIT in each case.

(require 'wt-tree)

;;  Test code, using maps from digit strings to the numbers they represent.

(define (wt-test)

  (define (make-map lo hi step)
    (let loop ((i lo) (map (make-wt-tree string-wt-type)))
      (if (> i hi)
          map
          (loop (+ i step) (wt-tree/add map (number->string i) i)))))

  (define (wt-tree->alist t)
    (wt-tree/fold (lambda (key datum rest) (cons (cons key datum) rest)) '() t))

  (define (try-all operation trees)
    (map (lambda (t1)
           (map (lambda (t2)
                  (operation t1 t2))
                trees))
         trees))

  (define (chunk tree)
    (let ((size  (wt-tree/size tree)))
      (if (< size 8)
          size
          (let* ((midpoint (if (even? size)
                               (/ size 2)
                               (/ (+ size 1) 2)))
                 (fulcrum  (wt-tree/index tree midpoint)))
            (list (chunk (wt-tree/split< tree fulcrum))
                  (list fulcrum)
                  (chunk (wt-tree/split> tree fulcrum)))))))

  (define (verify name result expected)
    (newline)
    (display "Test ") (display name)
    (if (equal? result expected)
        (begin
          (display " passed"))
        (begin
          (display " unexpected result")
          (newline)
          (display "Expected: " expected)
          (newline)
          (display "Got:      " result))))

  (let ((t1 (make-map 0 99 2))          ; 0,2,4,...,98
        (t2 (make-map 1 100 2))         ; 1,3,5,...,99
        (t3 (make-map 0 100 3)))        ; 0,3,6,...,99


    (verify 'alist (wt-tree->alist t3)  ;
            '(("0" . 0) ("12" . 12) ("15" . 15) ("18" . 18) ("21" . 21)
              ("24" . 24) ("27" . 27) ("3" . 3) ("30" . 30) ("33" . 33)
              ("36" . 36) ("39" . 39) ("42" . 42) ("45" . 45) ("48" . 48)
              ("51" . 51) ("54" . 54) ("57" . 57) ("6" . 6) ("60" . 60)
              ("63" . 63) ("66" . 66) ("69" . 69) ("72" . 72) ("75" . 75)
              ("78" . 78) ("81" . 81) ("84" . 84) ("87" . 87) ("9" . 9)
              ("90" . 90) ("93" . 93) ("96" . 96) ("99" . 99)))


    (verify 'union-sizes
            (try-all (lambda (t1 t2) (wt-tree/size (wt-tree/union t1 t2)))
                     (list t1 t2 t3))
            '((50 100 67) (100 50 67) (67 67 34)))

    (verify 'difference-sizes
            (try-all (lambda (t1 t2)
                       (wt-tree/size (wt-tree/difference t1 t2)))
                     (list t1 t2 t3))
            '((0 50 33) (50 0 33) (17 17 0)))

    (verify 'intersection-sizes
            (try-all (lambda (t1 t2)
                       (wt-tree/size (wt-tree/intersection t1 t2)))
                     (list t1 t2 t3))
            '((50 0 17) (0 50 17) (17 17 34)))

    (verify 'equalities
            (try-all (lambda (t1 t2)
                       (wt-tree/set-equal? (wt-tree/difference t1 t2)
                                           (wt-tree/difference t2 t1)))
                     (list t1 t2 t3))
            '((#t #f #f) (#f #t #f) (#f #f #t)))

    (verify 'indexing
            (chunk (make-map 0 99 1))
            '((((7 ("15") 5) ("20") (6 ("27") 4)) ("31")
               ((6 ("38") 5) ("43") (6 ("5") 4)))
              ("54")
              (((7 ("61") 5) ("67") (6 ("73") 4)) ("78")
               ((6 ("84") 5) ("9") (5 ("95") 4)))))
    (newline)))

(wt-test)

;;; Local Variables:
;;; eval: (put 'with-n-node 'scheme-indent-function 1)
;;; eval: (put 'with-n-node 'scheme-indent-hook 1)
;;; End:
