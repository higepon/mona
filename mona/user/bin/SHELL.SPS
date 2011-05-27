(import (rnrs) (monapi) (srfi :8) (mosh))

(monapi-name-add! "/servers/shell")

(let loop ()
  (receive msg (monapi-message-receive)
   (format #t "received ~a" msg)
   (loop)))
