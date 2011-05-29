(import (rnrs) (monapi) (srfi :8) (mosh)
    (match) (mosh file) (shorten)
    (only (mosh ffi) null-terminated-utf8->string string->utf8z))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (write str)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (write command-line)
        (match (string-split command-line #\space)
         [("ls" dir)
           (for-each
             (^f
               (write (string->utf8 f))
	       (monapi-stream-write (monapi-make-stream handle) (string->utf8 f)))
	     (directory-list dir))
           (monapi-stream-write (monapi-make-stream handle) #vu8(0))
           (monapi-message-reply from header MSG_OK)]
         [x
           (error 'shell "unknown command" x)]))
      (loop)]
    [else
      (format #t "received ~a" header)
      (loop)])))
