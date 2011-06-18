(import (rnrs)
        (monapi)
        (srfi :8)
        (mosh)
        (match)
        (mosh control)
        (mosh file)
        (shorten)
        (mosh process)
        (only (mosh ffi) null-terminated-utf8->string string->utf8z))

(define (ls stream dir)
  (for-each
    (^f
      (write f)
      (newline)
      (monapi-stream-write stream (string->utf8 (string-append f "\n"))))
    (directory-list dir)))


(define (ps stream)
  (monapi-stream-write stream (string->utf8 "tid name"))
  (for-each
    (^p
      (monapi-stream-write stream (string->utf8 (format "~d ~a\n" (assoc-ref p 'tid) (assoc-ref p 'name)))))
    (process-list))
  (monapi-stream-write stream (string->utf8 "tid name")))


(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (let1 stream (monapi-make-stream handle)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (ls stream dir)
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
           #;(apply call-process x)
           (format #t "unknown command ~a" x)
           (monapi-message-reply from header MSG_OK)]))
      (loop)]
    [else
      (format #t "received ~a\n" header)
      (loop)]))))
