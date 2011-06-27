(library (shell path)
         (export complement)
         (import (rnrs) (mosh) (mosh file) (mosh control) (shorten))

(define (find-executable dir name)
  (aif (filter (string->regexp (format "~a\\.EX" name))
         (directory-list dir))
    (if (pair? it) (car it) #f)))

(define (complement command)
  (cond
   [(find-executable "/APPS/" command) =>
     (^p (string-append "/APPS/" p))]
   [else command]))

)
