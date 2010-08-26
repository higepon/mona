(import (rnrs)
        (mosh)
        (shorten)
        (match)
        (srfi :26)
        (mosh file))

(define (file->sexp-list file)
  (with-input-from-file file
    (lambda ()
      (let loop ([line (read)]
                 [ret '()])
        (cond
         [(eof-object? line) (reverse ret)]
         [else
          (loop (read) (cons line ret))])))))

(define header
"/*
    Title: Errors

    Pre-defined system errors.

    <M_OK> indicates a successful return and is guaranteed to be zero.
    Other errors are guaranteed to be negative value.
*/

#ifndef _MONA_ERROR_
#define _MONA_ERROR_

#include \"sys/deprecated.h\"

")

(define footer "#endif\n")

(define (comment-header name msg . description)
  (format #t
"/*
    constant: ~a

    Indicates ~a
*/
" name msg))


(define (main args)
  (display "//\n//   Gerated by script. Don't edit this file\n//\n\n")
  (display header)
  (let loop ([def* (file->sexp-list (cadr args))]
             [code 0])
    (cond
     [(null? def*) '()]
     [else
      (match (car def*)
      [('define-error name msg code description)
       (comment-header name msg description)
       (format #t "#define ~a ~d\n\n" name code)
       (loop (cdr def*) (- code 1))]
      [('define-error name msg code)
       (comment-header name msg)
       (format #t "#define ~a ~d\n\n" name code)
       (loop (cdr def*) (- code 1))]
      [('define-error name msg)
       (comment-header name msg)
       (format #t "#define ~a ~d\n\n" name code)
       (loop (cdr def*) (- code 1))])]))
  (display footer))

(main (command-line))
