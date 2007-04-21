;; (load "./lib/scheme.scm")
;; (load "./lib/unittest.scm")
(load "/LIBS/SCHEME/scheme.scm")
(load "/LIBS/SCHEME/unittest.scm")


(define (filter pred lis)           ; Sleazing with EQ? makes this one faster.
  (let recur ((lis lis))
    (if (null? lis) lis         ; Use NOT-PAIR? to handle dotted lists.
    (let ((head (car lis))
          (tail (cdr lis)))
      (if (pred head)
          (let ((new-tail (recur tail)))    ; Replicate the RECUR call so
        (if (eq? tail new-tail) lis
            (cons head new-tail)))
          (recur tail))))))         ; this one can be a tail call.



(define find (lambda (pred list)
               (call/cc
                (lambda (return)
                  (for-each
                   (lambda (e) (if (pred e) (return e)))
                   list)
                  #f))))

(define (remove pred l)
  (filter (lambda (s) (not (pred s))) l))

(define (make-directory-entry name directoryp)
  (let ((children '()))
    (define (add-child child)
      (set! children (cons child children)))
    (define (has-child? name)
      (find (lambda (d) (string=? (d 'name) name)) children))
    (define (dispatch m)
      (cond ((eq? m 'name) name)
            ((eq? m 'directory?) directoryp)
            ((eq? m 'children) children)
            ((eq? m 'add-child) add-child)
            ((eq? m 'has-child?) has-child?)
            (else (print "make-directory-entry error"))))
    dispatch))

(define (make-directory-session root current-directory)
  (define (directory-has-child? dir paths)
    (if (null? paths)
        #t
        (let ((next-dir ((dir 'has-child?) (car paths))))
          (if (and next-dir (next-dir 'directory?))
              (directory-has-child? next-dir (cdr paths))
              #f))))
  (define (directory-exist? path)
    (define (start-with-slash? path)
      (and (> (string-length path) 0) (char=? (string-ref path 0) #\/)))
    (let ((paths (string-split path #\/)))
      (if (start-with-slash? path)
          (if (= 1 (string-length path))
              #t
              (directory-has-child? root (cdr paths)))
          (directory-has-child? current-directory paths))))
  (define (dispatch m)
    (cond ((eq? m 'current-directory) current-directory)
          ((eq? m 'root) root)
          ((eq? m 'directory-exist?) directory-exist?)
          (else (print "make-directory-session error"))))
  dispatch)

(let* ((root (make-directory-entry "/" #t))
       (hige (make-directory-entry "hige" #t))
       (hage (make-directory-entry "hage" #f))
       (pon (make-directory-entry "pon" #t))
       (d (make-directory-session root hige)))
  ((root 'add-child) hige)
  ((root 'add-child) hage)
  ((hige 'add-child) pon)
  (assert-check-false "directory-session"
                      ((d 'directory-exist?) "/hage")
                      ((d 'directory-exist?) "/hage/pon")
                      ((d 'directory-exist?) "")
                      )
  (assert-check-true "directory-session"
                     ((d 'directory-exist?) "/")
                     ((d 'directory-exist?) "/hige")
                     ((d 'directory-exist?) "/hige/pon")
                     ((d 'directory-exist?) "pon")
                     ))
(total-report)

;(for-each (lambda (d) (display (d 'name)) (print (d 'directory?))) (directory-entries "/home/taro/src"))

(define (create-directory-tree root path)
  (print path)
  (let ((directories (remove (lambda (d) (or (string=? "." (d 'name)) (string=? ".." (d 'name)))) (directory-entries path))))
    (for-each
     (lambda (d)
       ((root 'add-child) d)
       (if (d 'directory?)
           (create-directory-tree d (string-append (string-append path "/") (d 'name)))))
     directories)))


(define root (make-directory-entry "/" #t))
;(create-directory-tree root "/home/taro/mona/bin")
(create-directory-tree root "/SERVERS")

(define (print-directory root space)
(for-each (lambda (d) (display space )(print (d 'name)) (if (d 'directory?) (print-directory d (string-append space " ")))) (root 'children)))

(print-directory root "")
