; "trnscrpt.scm", transcript functions for Scheme.
; Copyright (c) 1992, 1993, 1995 Aubrey Jaffer
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

(define transcript:port #f)
;@
(define (transcript-on filename)
  (set! transcript:port (open-output-file filename)))
;@
(define (transcript-off)
  (if (output-port? transcript:port)
      (close-output-port transcript:port))
  (set! transcript:port #f))
;@
(define read-char
  (let ((read-char read-char) (write-char write-char))
    (lambda opt
      (let ((ans (apply read-char opt)))
	(cond ((eof-object? ans))
	      ((output-port? transcript:port)
	       (write-char ans transcript:port)))
	ans))))
;@
(define read
  (let ((read read) (write write) (newline newline))
    (lambda opt
      (let ((ans (apply read opt)))
	(cond ((eof-object? ans))
	      ((output-port? transcript:port)
	       (write ans transcript:port)
	       (if (eqv? #\newline (apply peek-char opt))
		   (newline transcript:port))))
	ans))))
;@
(define write-char
  (let ((write-char write-char))
    (lambda (obj . opt)
      (apply write-char obj opt)
      (if (output-port? transcript:port)
	  (write-char obj transcript:port)))))
;@
(define write
  (let ((write write))
    (lambda (obj . opt)
      (apply write obj opt)
      (if (output-port? transcript:port)
	  (write obj transcript:port)))))
;@
(define display
  (let ((display display))
    (lambda (obj . opt)
      (apply display obj opt)
      (if (output-port? transcript:port)
	  (display obj transcript:port)))))
;@
(define newline
  (let ((newline newline))
    (lambda opt
      (apply newline opt)
      (if (output-port? transcript:port)
	  (newline transcript:port)))))
