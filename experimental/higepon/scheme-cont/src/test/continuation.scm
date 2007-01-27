(define cont '())
(define f (lambda()
            1
            (call/cc (lambda (c) (set! cont c) 2))))

(assert-check-true "lambda: continuation"
                   (= (f) 2)
                   (= (cont 3) 3))

(define num 0)
(define cont '())
(define f (lambda ()
            1
            (call/cc (lambda (c) (set! cont c)))
            2
            (set! num (+ num 1))
            num))

(assert-check-true "lambda: continuation 2"
                   (= (f) 1)
                   (= (cont 1) 2)
                   (= (cont 1) 3))

(define cont '())
(define f (lambda ()
            1
            ((lambda ()
               2
               (call/cc (lambda (c) (set! cont c)))
               3))
            4))

(assert-check-true "lambda: continuation 3"
                   (= (f) 4)
                   (= (cont 1) 4))

(define cont '())
(define num0 0)
(define num1 0)
(define num2 0)
(define num3 0)

(define f (lambda ()
            1
            ((lambda ()
               ((lambda ()
                  2
                  (set! num0 (+ 1 num0))
                  (call/cc (lambda (c) (set! cont c)))
                  (set! num1 1)
                  3))
               (set! num2 1)
               4))
            (set! num3 1)
            5))

(assert-check-true "lambda: continuation 4"
                   (= (f) 5)
                   (= (cont 1) 5)
                   (= 1 num0)
                   (= 1 num1)
                   (= 1 num2)
                   (= 1 num3))



