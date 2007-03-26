; "withfile.scm", with-input-from-file and with-output-to-file for Scheme
; Copyright (c) 1992, 1993 Aubrey Jaffer
;
;Permission to copy this software, to modify it, to redistribute it,
;to distribute modified versions, and to use it for any purpose is
;granted, subject to the following restrictions and understandings.
;
;1.  Any copy made of this software must include this copyright notice
;in full.
;
;2.  I have made no warranty or representation that the operation of
;this software will be error-free, and I am under no obligation to
;provide any services, by way of maintenance, update, or otherwise.
;
;3.  In conjunction with products arising from the use of this
;material, there shall be no use of my name in any advertising,
;promotional, or sales literature without prior written consent in
;each case.

(require 'dynamic-wind)

(define withfile:current-input (current-input-port))
(define withfile:current-output (current-output-port))
;@
(define (current-input-port) withfile:current-input)
(define (current-output-port) withfile:current-output)
;@
(define (with-input-from-file file thunk)
  (define oport withfile:current-input)
  (define port (open-input-file file))
  (dynamic-wind (lambda () (set! oport withfile:current-input)
			   (set! withfile:current-input port))
		(lambda() (let ((ans (thunk))) (close-input-port port) ans))
		(lambda() (set! withfile:current-input oport))))
;@
(define (with-output-to-file file thunk)
  (define oport withfile:current-output)
  (define port (open-output-file file))
  (dynamic-wind (lambda() (set! oport withfile:current-output)
		          (set! withfile:current-output port))
		(lambda() (let ((ans (thunk))) (close-output-port port) ans))
		(lambda() (set! withfile:current-output oport))))
;@
(define peek-char
  (let ((peek-char peek-char))
    (lambda opt
      (peek-char (if (null? opt) withfile:current-input (car opt))))))
;@
(define read-char
  (let ((read-char read-char))
    (lambda opt
      (read-char (if (null? opt) withfile:current-input (car opt))))))
;@
(define read
  (let ((read read))
    (lambda opt
      (read (if (null? opt) withfile:current-input (car opt))))))
;@
(define write-char
  (let ((write-char write-char))
    (lambda (obj . opt)
      (write-char obj (if (null? opt) withfile:current-output (car opt))))))
;@
(define write
  (let ((write write))
    (lambda (obj . opt)
      (write obj (if (null? opt) withfile:current-output (car opt))))))
;@
(define display
  (let ((display display))
    (lambda (obj . opt)
      (display obj (if (null? opt) withfile:current-output (car opt))))))
;@
(define newline
  (let ((newline newline))
    (lambda opt
      (newline (if (null? opt) withfile:current-output (car opt))))))
;@
(define force-output
  (let ((force-output force-output))
    (lambda opt
      (force-output (if (null? opt) withfile:current-output (car opt))))))
