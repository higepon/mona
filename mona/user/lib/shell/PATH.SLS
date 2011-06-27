(library (shell path)
         (export complement)
         (import (rnrs))

(define (complement command) (string-append "/APPS/" command ".EX5"))


)
