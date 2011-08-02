(library (shell)
         (export call-process+ start-process+ ps ls cat help)
         (import (rnrs) (shorten) (monapi) (mosh) (mosh file) 
                 (shell path) (mosh process) (only (srfi :13) string-join string-contains))

(define (ls stream dir)
  (for-each
    (^f
      (monapi-stream-write stream (string->utf8 (string-append f "\n"))))
    (directory-list dir)))

(define (help stream)
  (write-string stream
    "Commands\n    ps\n    ls [dir]\n    cat [path]\n    kill [tid]\n    application executable\n\n\n"))
(define (cat stream path)
  (call-with-port (open-input-file path)
    (^p
      (let loop ([line (get-line p)])
        (cond
          [(eof-object? line) '()]
          [else
            (write-string stream line)
            (write-string stream "\n")
            (loop (get-line p))])))))

(define (ps stream)
  (write-string stream "tid name")
  (for-each
    (^p
      (write-string stream (format "~d ~a\n" (assoc-ref p 'tid) (assoc-ref p 'name))))
    (process-list))
  (write-string stream "tid name"))

(define (write-string stream s)
  (monapi-stream-write stream (string->utf8 s)))

(define (execute-process+ proc command* . stream)
  (apply proc (string-join (complete-path command*) " ") stream))

(define (call-process+ command* . stream)
  (apply execute-process+ call-process command* stream))

(define (start-process+ command* . stream)
  (apply execute-process+ start-process command* stream))

(define (complete-path command*)
  (cond
    [(and (pair? command*) (not (string-contains (car command*) "/")))
      (cons (complement (string-upcase (car command*))) (cdr command*))]
    [else command*]))

)
