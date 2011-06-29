(import (rnrs)
        (shell path)
        (shorten)
        (mosh)
        (mosh process))

(define test* '(TSTREAM TBLOCK TMOSH TPROCESS TCLIP TFILE TVIRTIO TSTACK TGMP TMISC TMUTEX TCOND
                TSTDIO TMSG TSYSCALL TRAMDISK TDIE TSOCK))

(define (run-all-tests)
  (for-each (^t (call-process (complement (symbol->string t)))) test*)
  (print "all tests done"))

(run-all-tests)
