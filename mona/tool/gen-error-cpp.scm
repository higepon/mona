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
"#include <monapi.h>
#include <sys/error.h>

using namespace MonAPI;

const char* monapi_error_string(int error)
{
    switch(error) {
")

(define footer "    default:\n     _logprintf(\"unknown error %d\\n\", error);      return \"unknown error see log\";\n    }\n}")


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
       (format #t "    case ~a:\n        return  \"~a\";\n" name msg)
       (loop (cdr def*) (- code 1))]
      [('define-error name msg code)
       (format #t "    case ~a:\n        return \"~a\";\n" name msg)
       (loop (cdr def*) (- code 1))]
      [('define-error name msg)
       (format #t "    case ~a:\n        return \"~a\";\n" name msg)
       (loop (cdr def*) (- code 1))])]))
  (display footer))

(main (command-line))
