(import (rnrs)
        (monapi)
        (srfi :8)
        (mosh)
        (match)
        (mosh control)
        (mosh file)
        (shorten)
        (mosh process)
        (only (srfi :13) string-contains string-join)
        (only (mosh ffi) null-terminated-utf8->string string->utf8z))

(define (ls stream dir)
  (for-each
    (^f
      (write f)
      (newline)
      (monapi-stream-write stream (string->utf8 (string-append f "\n"))))
    (directory-list dir)))

(define (write-string stream s)
  (monapi-stream-write stream (string->utf8 s)))

(define (ps stream)
  (write-string stream "tid name")
  (for-each
    (^p
      (write-string stream (format "~d ~a\n" (assoc-ref p 'tid) (assoc-ref p 'name))))
    (process-list))
  (write-string stream "tid name"))

(define (complete-path command*)
  (cond
    [(and (pair? command*) (not (string-contains (car command*) "/")))
      (if (file-exists? (string-append "/APPS/" (car command*)))     
          (cons (string-append "/APPS/" (car command*)) (cdr command*))
          command*)]
    [else command*]))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (let1 stream (monapi-make-stream handle)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "ls failed ~a" dir)))]]
           (ls stream dir))
           (monapi-message-reply from header MSG_OK)]
         [("ps")
           (ps stream)
           (monapi-message-reply from header MSG_OK)]
         [("kill" id)
           (monapi-stream-write stream (string->utf8 (format "~a\n" (if (process-terminate! (string->number id))
                                                                        "process killed"
                                                                        "can't kill the process"))))
           (monapi-message-reply from header MSG_OK)]
         [x
           (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "call-process failed : ~a" x)))]]
           (apply call-process (list (string-join (complete-path x) " ") stream)))
#;           (format #t "unknown command ~a" x)
           (monapi-message-reply from header MSG_OK)]))
      (loop)]
    [else
      (format #t "received ~a\n" header)
      (loop)]))))
