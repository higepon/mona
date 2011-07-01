(import (rnrs)
        (shorten)
        (shell))

(define test* '(tblock
                tstream
                tblock
                tmosh
                tprocess
                tclip
                tfile
                tvirtio
                tstack
                tgmp
                tmisc
                tmutex
                tcond
                tstdio
                tmsg
                tsyscall
                tramdisk
                tdie
                tsock))

(for-each (^n (call-process+ (list (symbol->string n)))) test*)
(display "all tests done\n")
