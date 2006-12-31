(define-syntax when
  (syntax-rules ()
    ((_ pred a b)
     (if pred (begin a b)))))

(define x 3)

(when (= x 3)
      (display "x=3")
      (display "\n"))


(define-syntax decf
  (syntax-rules ()
    ((_ x) (begin (set! x (- x 1)) x))
    ((_ x dx) (begin (set! x (- x dx)) x))))

(decf x)
(display x)

(decf x 3)
(display x)

(define (p x)
  (display x)
  (newline))

(let* ((a 3) (b (+ a 1)))
  (p a)
  (p b))

(let ((a 3))
  (let ((b (+ a 1)))
    (p a)
    (p b)))

(define-syntax my-let*
  (syntax-rules ()
    ((_ ((a b)) z ...) (let ((a b)) z ...))
    ((_ ((a b) (c d) ...) z ...)
     (let ((a b))
       (my-let* ((c d) ...)
         z ...)))))
;;    ((_ (a b) z) (my-let* (a) (my-let* (b) z)))))
;;    ((_ ((a x)) z) (let ((a x)) z))))
;;    ((_ ((a x) (b y) ...) z) (my-let* ((a x)) (my-let* ((b y)) ...)))))


(my-let* ((a 99))
         (p a))

(my-let* ((a 99) (b (+ 1 a)) (c (+ b 1)))
         (p a)
         (p b)
         (p c))

(define-syntax pp
  (syntax-rules ()
  ((_ a) (begin (display a) (newline)))
  ((_ a b ...) (begin (pp a) (pp b ...)))))

(pp "hoge" "hige" "huga")

(let-syntax ((hello (syntax-rules ()
  ((_ a) (pp "hello" a)))))
  (hello "higepon"))

(define-syntax my-if
  (syntax-rules (my-then my-else)
    ((_ a my-then b) (if a b))
    ((_ a my-then b my-else c) (if a b c))))

(define y 4)
(my-if (= y 3)
  my-then (pp "3")
  my-else (pp "not 3"))



(define-syntax show
  (syntax-rules ()
    ((_ a) (display a))))

(show "higeAAA")

