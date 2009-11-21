(import (rnrs)
        (mosh)
        (mosh control)
        (only (mosh concurrent) sleep)
        (mosh process)
        (mosh file))

(define-syntax with-color
  (lambda (x)
    (syntax-case x ()
      [(_ color expr more ...)
       (if (string=? (host-os) "win32")
           #'(begin expr more ...)
           #'(dynamic-wind
                 (lambda () (display color))
                 (lambda () expr more ...)
                 (lambda () (display "\x1b;[m")))]))))

(define-syntax with-color-green
  (lambda (x)
    (syntax-case x ()
      [(_ expr more ...)
       #'(with-color "\x1b;[0;32m" expr more ...)])))

(define-syntax with-color-red
  (lambda (x)
    (syntax-case x ()
      [(_ expr more ...)
       #'(with-color "\x1b;[0;31m" expr more ...)])))

(define test-results-file "/tmp/mona_serial.log")
(define pid-file "/tmp/mona.pid")

(let loop ()
  (let1 text (file->string test-results-file)
    (when (#/all tests done/ text)
      (let* ([results (string-split text #\newline)]
             [passed (filter #/test passed/ results)]
             [each-errors (filter #/MUnit:/ results)]
             [failed (filter #/failed/ results)])
        (cond
         [(and (null? each-errors) (null? failed))
          (with-color-green
           (for-each print results))]
         [else
          (with-color-red
           (for-each print each-errors)
           (for-each print failed))]))
      (spawn "kill" (list (number->string (read (open-input-file pid-file)))))
      (exit 0))
    (sleep 100)
    (loop)))
