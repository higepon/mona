(import (rnrs)
        (mosh control)
        (match)
        (mosh)
        (system)
        (shorten)
        (facebook))

(define temp-file "/USER/TEMP/fb.data")
;(define temp-file "fb.data")

(when (file-exists? temp-file)
    (delete-file temp-file))

(let1 json (fb-news "")
  (call-with-port
   (open-output-file temp-file)
   (^p
   (for-each
    (^(m)
      (cond
       [(and (assoc-ref m "message") (assoc-ref m "from"))
        (format p "~a\n~a\n" (assoc-ref (vector->list (assoc-ref m "from")) "name") (assoc-ref m "message"))]
       [else '()]))
    json))))
