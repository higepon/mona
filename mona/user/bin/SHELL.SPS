(import (rnrs)
        (monapi)
        (srfi :8)
        (mosh)
        (match)
        (mosh control)
        (mosh file)
        (shorten)
        (only (mosh ffi) null-terminated-utf8->string string->utf8z))

(monapi-name-add! "/servers/shell")

(define (ls stream dir)
  (for-each
    (^f
      (monapi-stream-write stream (string->utf8 f))
      (monapi-stream-write stream (string->utf8 "\n")))
    (directory-list dir))
    (monapi-stream-write stream #vu8(0)))

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
         [x
           (error 'shell "unknown command" x)]))
      (loop)]
    [else
      (format #t "received ~a\n" header)
      (loop)]))))
