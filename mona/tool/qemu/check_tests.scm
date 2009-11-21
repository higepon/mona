; check_tests.scm
;
;   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
;
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions
;   are met:
;
;   1. Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;
;   2. Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
;   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
;   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
;   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
;   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; Check resuls of Mona test.

; 1. run qemu as daemon.
; 2. wait for test results are written in /tmp/mona_serial.log.
; 3. check test results.
; 4. kill qemu by pid.

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
           (for-each print passed))]
         [else
          (with-color-red
           (for-each print each-errors)
           (for-each print failed))]))
      (spawn "kill" (list (number->string (read (open-input-file pid-file)))))
      (exit 0))
    (sleep 100)
    (loop)))
