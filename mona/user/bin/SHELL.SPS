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
      (monapi-stream-write stream (string->utf8 f))
      (monapi-stream-write stream (string->utf8 "\n")))
    (directory-list dir))
    (monapi-stream-write stream #vu8(0)))

(define (ps stream)
  (monapi-stream-write stream (string->utf8 "tid name"))
  (for-each
    (^p
      (monapi-stream-write stream (string->utf8 (assoc-ref p 'name)))
      (monapi-stream-write stream (string->utf8 "\n")))
    (process-list))
  (monapi-stream-write stream (string->utf8 "tid name"))
  (monapi-stream-write stream #vu8(0)))

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
         [x
           (error 'shell "unknown command" x)]))
      (loop)]
    [else
      (format #t "received ~a\n" header)
      (loop)]))))
