(import (rnrs)
        (srfi :8)
        (mosh)
        (mosh test)
        (monapi))
;(display "hello")
;(display (monapi-message-send (monapi-name-whereis "/servers/shell") MSG_STOP 2 3 4 #vu8(1 2 3)))

(define (ls dir)
  (let ([shell (monapi-name-whereis "/servers/shell")])
    (receive a (monapi-message-send-receive shell MSG_TEXT 0 0 0 (string->utf8 "ls"))
	(format #t "test received ~a\n" a))))

(test-equal '("/APPS/HELLO.EX5") (ls "/APPS/HELLO.EX5"))

(test-results)
