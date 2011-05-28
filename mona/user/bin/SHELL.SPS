(import (rnrs) (monapi) (srfi :8) (mosh)
    (match) (mosh file)
    (only (mosh ffi) null-terminated-utf8->string))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (format #t "~a ~a \n" header MSG_STOP)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (display "write start\n")
           (monapi-stream-write (monapi-make-stream handle) #vu8(78)) 
	   (display "write end\n")
           (write (directory-list dir))]
         [x
            (error 'shell "unknown command" x)]))
      (monapi-message-reply from header MSG_OK)
      (display "shell stopped\n")]
    [else
      (format #t "received ~a" header)]))
   (loop))
