(import (rnrs) (monapi) (srfi :8) (mosh)
    (match) (mosh file) (shorten)
    (only (mosh ffi) null-terminated-utf8->string string->utf8z))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (for-each
             (^f
	       (monapi-stream-write (monapi-make-stream handle) (string->utf8z f)))
	     (directory-list dir))
           (monapi-message-reply from header MSG_OK)]
         [x
           (error 'shell "unknown command" x)]))
      (loop)]
    [else
      (format #t "received ~a" header)])))
