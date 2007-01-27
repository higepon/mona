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
