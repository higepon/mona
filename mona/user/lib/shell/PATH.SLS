(library (shell path)
         (export complement find-app-dir)
         (import (rnrs) (mosh) (mosh file) (mosh control) (shorten))

(define (find-executable dir name)
  (let1 regexp (string->regexp (if (#/EX.$/ name) name (format "^~a\\.EX" name)))
    (aif (filter regexp (directory-list dir))
         (if (pair? it) (car it) #f))))

(define (find-app-dir dir name)
  (let1 regexp (string->regexp (aif (#/(.*).EX.$/ name) (it 1) (format "^~a\\.APP" name)))
    (aif (filter regexp (directory-list dir))
         (if (pair? it) (car it) #f))))

(define (complement command)
  (let1 dir* '("/APPS/" "/APPS/MONAGUI/")
    (let loop ([dir* dir*])
      (cond
        [(null? dir*) command]
        [(find-executable (car dir*) command) =>
          (^p (string-append (car dir*) p))]
        [else
          (loop (cdr dir*))]))))


)
