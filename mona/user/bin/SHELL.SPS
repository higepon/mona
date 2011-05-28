(import (rnrs) (monapi) (srfi :8) (mosh))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive (from header arg1 arg2 arg3 str) (monapi-message-receive)
   (format #t "~a ~a \n" header MSG_STOP)
   (cond
    [(= header MSG_TEXT)
      (monapi-message-reply from header MSG_OK)
      (display "shell stopped\n")]
    [else
      (format #t "received ~a" header)]))
   (loop))
