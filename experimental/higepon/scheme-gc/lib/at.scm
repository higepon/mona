;; (define cont '())

;; (define (sample)
;;   (print 1)
;;   (print 2)
;;   (call/cc (lambda (c) (set! cont c)))
;;   (print 3)
;;   (print 4)
;;   (print 5))

;; (sample)

;; ;(cont 1)

;; (define mona-timer-on #t)
;; (define mona-timer-cont cont)
;; (define mona-timer-ms 10000)

;; (define (mona-timer)
;;   (if mona-timer-on
;;       (begin (set! mona-timer-ms (- mona-timer-ms 100))
;;       (print mona-timer-ms)
;;       (if (negative? mona-timer-ms)
;;           (begin
;;             (display "in")
;;             (set! mona-timer-on #f)
;;             (mona-timer-cont 1)
;;             )
;;           #f))))

;; (mona-timer)
;; (mona-timer)
;; (mona-timer)


(define mona-timer-list '())

(define (mona-timer-add cont ms)
  (set! mona-timer-list (append mona-timer-list (cons cont ms))))

(mona-timer-add (lambda () 10) 100)

(print mona-timer-list)

(mona-timer-add (lambda () 10) 200)

(print mona-timer-list)


