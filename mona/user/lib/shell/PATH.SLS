(library (shell path)
         (export complement
                 find-app-dir ;; for testability
         )
         (import (rnrs) (mosh) (mosh file) (mosh control) (shorten))

(define (find-executable dir name)
  (let1 regexp (string->regexp (if (#/EX.$/ name) name (format "^~a\\.EX" name)))
    (or
      (aif (find regexp (directory-list dir))
           (string-append dir it)
      (aif (find-app-dir dir name)
           (find-executable (string-append dir it "/")  name)
            #f)))))

(define (find-app-dir dir name)
  (let1 regexp (string->regexp (aif (#/(.*).EX.$/ name) (format "^~a\\.APP" (it 1)) (format "^~a\\.APP" name)))
    (find regexp (directory-list dir))))

(define (complement command)
  (let1 dir* '("/APPS/" "/APPS/MONAGUI/")
    (let loop ([dir* dir*])
      (cond
        [(null? dir*) command]
        [(find-executable (car dir*) command) =>
          (^p p)]
        [else
          (loop (cdr dir*))]))))


)
