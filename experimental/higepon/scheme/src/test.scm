;; valgrind用
;; (begin
;; (define a 3)
;; (define b 4)
;; (define c (+ a b))
;; (display c)
;; )


;; 死ぬコード
(begin
  (define a (lambda (num)
              (display (cons 1 2)) (display num)(a (+ num 1))))
  (a 0)
  )

;;261855で死んだ

;; (begin
;; (define a 3)
;; (define b 4)
;; (define c (+ a b))
;; (display c)
;; )
