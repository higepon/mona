;; valgrind用
;; (begin
;; (define a 3)
;; (define b 4)
;; (define c (+ a b))
;; (display c)
;; )


;; 死ぬコード
;; (begin
;;   (define a (lambda (num)
;;               (display (cons 1 2)) (display num)(a (+ num 1))))
;;   (a 0)
;;   )

;;(begin
  (define ok (lambda ()
               (display "OK\n")))

(define not-ok (lambda ()
             (display "Not OK\n")))


(define assert (lambda (condition msg)
                 (if condition
                     (ok)
                     (not-ok))))

(assert (= 3 3) "hoge")
;;)

;;261855で死んだ

;; (begin
;; (define a 3)
;; (define b 4)
;; (define c (+ a b))
;; (display c)
;; )
