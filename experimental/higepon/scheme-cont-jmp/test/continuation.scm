(define for-loop (lambda (max)
  (let ((i 0) (cont #f))
    (if (> max (call/cc (lambda (c) (set! cont c) i)))
        (begin
          (display i)
          (newline)
          (set! i (+ i 1))
          (cont i))))))

(for-loop 5)

;; (define cont '())
;; (define f (lambda ()
;;             (let ((cont '()))
;;               (if (call/cc (lambda (c) (set! cont c) #t))
;;                   ((lambda ()
;;                      (display "hello")
;;                      (newline)
;;                      (cont #f)))
;;                   (display "continuation")))))

;; (f)

;; (define cont '())
;; (define f (lambda()
;;             1
;;             (call/cc (lambda (c) (set! cont c) 2))))

;; (assert-check-true "lambda: continuation"
;;                    (= (f) 2)
;;                    (= (cont 3) 3))

;; (define num 0)
;; (define cont '())
;; (define f (lambda ()
;;             1
;;             (call/cc (lambda (c) (set! cont c)))
;;             2
;;             (set! num (+ num 1))
;;             num))

;; (assert-check-true "lambda: continuation 2"
;;                    (= (f) 1)
;;                    (= (cont 1) 2)
;;                    (= (cont 1) 3))

;; (define cont '())
;; (define f (lambda ()
;;             1
;;             ((lambda ()
;;                2
;;                (call/cc (lambda (c) (set! cont c)))
;;                3))
;;             4))

;; (assert-check-true "lambda: continuation 3"
;;                    (= (f) 4)
;;                    (= (cont 1) 4))

;; (define cont '())
;; (define num0 0)
;; (define num1 0)
;; (define num2 0)
;; (define num3 0)

;; (define f (lambda ()
;;             1
;;             ((lambda ()
;;                ((lambda ()
;;                   2
;;                   (set! num0 (+ 1 num0))
;;                   (call/cc (lambda (c) (set! cont c)))
;;                   (set! num1 (+ num1 1))
;;                   3))
;;                (set! num2 (+ 1 num2))
;;                4))
;;             (set! num3 (+ 1 num3))
;;             5))

;; (assert-check-true "lambda: continuation 4"
;;                    (= (f) 5)
;;                    (= 1 num0)
;;                    (= 1 num1)
;;                    (= 1 num2)
;;                    (= 1 num3)
;;                    (= (cont 1) 5)
;;                    (= 2 num0)
;;                    (= 2 num1)
;;                    (= 2 num2)
;;                    (= 2 num3))

;; (define cont '())
;; (define f (lambda (poo)
;;             (set! poo (+ poo 1))
;;             (call/cc (lambda (c) (set! cont c)))
;;             (set! poo (+ poo 1))
;;             poo))

;; (assert-check-true "lambda: continuation 5"
;;                    (= (f 3) 5)
;;                    (= (cont #f) 6)
;;                    (= (cont #f) 7)
;;                    (= (cont #f) 8))


;; (define conti '())

;; (define f (lambda ()
;;             1
;;             ((lambda (puga)
;;                (set! puga (+ puga 1))
;;                (call/cc (lambda (c) (set! conti c)))
;;                (set! puga (+ puga 1))
;;                (display "*************************************************")
;;                puga)
;;              100)
;;                (display "----------------------------------")
;; ))
;; (newline)
;; (display (f))
;; (newline)
;; (display "***")
;; (display (conti 1))
;; (newline)

;; (display (cont 1))
;; (display (cont 1))

;(display (cont 1))
;(display (cont 1))

;; (assert-check-true "lambda: continuation 5"
;;                    (= (f) 102)
;;                    (= (cont 1) 103)
;;                    (= (cont 1) 104))
