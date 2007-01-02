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

(define-syntax pp
  (syntax-rules ()
  ((_ a) (begin (display a)))
  ((_ a ...) (begin (pp a) (pp ...)))))


(define-syntax p
  (syntax-rules ()
    ((_ a) (display a))))

(define-syntax my-if
  (syntax-rules (my-then my-else)
    ((_ a my-then b) (if a b))
    ((_ a my-then b my-else c) (if a b c))))

(define y 4)
(my-if (= y 3)
  my-then (pp "3")
  my-else (pp "not 3\n" "hige\n" "pon\n" "desu"))
