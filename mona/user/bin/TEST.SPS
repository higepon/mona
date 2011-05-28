(import (rnrs)
        (srfi :8)
        (mosh)
        (mosh test)
	(only (mosh ffi) string->utf8z)
        (mosh control)
        (monapi))

;; hold the stream not to GC-ed.
(define out (monapi-make-stream))

(define handle (monapi-stream-handle out))
(define shell (monapi-name-whereis "/servers/shell"))

(define (ls dir)
  (receive (from header . rest) (monapi-message-send-receive shell MSG_TEXT handle 0 0 (string->utf8z (string-append "ls " dir)))
    (let1 ret (utf8->string (monapi-stream-read out 64000))
      ret)))

(test-true (#/GUI\.DL./ (ls "/LIBS/")))

(test-results)
