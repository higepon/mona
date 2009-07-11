(import (rnrs)
        (match)
        (mosh file)
        (mosh))

(call-with-input-file "/home/taro/contrib/packet.log"
  (lambda (port)
  (let loop ([sexp (read port)]
             [accum-line #f]
             [accum-usec #f])
  (cond
   [(eof-object? sexp) '()]
   [else
    (match sexp
      [("main.cpp" line)
       (match (read port)
         [('time sec ': usec)
          (if accum-usec
              (begin
                (format #t "~d-~d usec=~d\n" accum-line line (- (+ (* sec 1000 1000) usec) accum-usec))
                (loop (read port) line (+ (* sec 1000 1000) usec)))
              (loop (read port) line (+ (* sec 1000 1000) usec)))
          ]
         [else
          (loop (read port) accum-line accum-usec)])]
      [else
       (loop (read port) accum-line accum-usec)]))))))

