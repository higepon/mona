(import (rnrs)
        (srfi :8)
        (mosh)
        (mosh test)
	(only (mosh ffi) string->utf8z)
        (monapi))
;(display "hello")
;(display (monapi-message-send (monapi-name-whereis "/servers/shell") MSG_STOP 2 3 4 #vu8(1 2 3)))

(define out (monapi-make-stream))
(define handle (monapi-stream-handle out))

(format #t "handle=<~a>\n" handle)

(display (monapi-stream-write out #vu8(123)))

(define (ls dir)
  (let ([shell (monapi-name-whereis "/servers/shell")])
    (receive (from header . rest) (monapi-message-send-receive shell MSG_TEXT handle 0 0 (string->utf8z "ls /USER/"))
	(display "read start\n")
        (format #t "\nstream-read ~a\n" (monapi-stream-read out 4))
	(format #t "test received ~a\n" (message->string header)))))

(test-equal '("/APPS/HELLO.EX5") (ls "/APPS/HELLO.EX5"))

(test-results)
