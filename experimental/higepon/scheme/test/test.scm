(load "./test/scheme.scm")
(display "hoge")
(newline)
(display "hoge")

(define a 1234)

(set! a (+ a 1))

(display a)


(define *test-track-progress* #t)  ;; for locationg SEGV point
(define *total-testsuites* 1)  ;; TODO: introduce test suites and defaults to 0
(define *total-testcases* 1)   ;; TODO: introduce testcase and defaults to 0
(define *total-tests* 1)       ;; TODO: introduce test group and defaults to 0
(define *total-failures*  0)
(define *total-assertions* 0)
(define *total-errors* 0) ;; TODO: recover unintended error and increment this
(define test-filename "unspecified")

(define report-error
  (lambda (msg)
      (display "failed: ")
      (display msg)
      (newline)))

(define report-inequality
  (lambda (expected actual)
    (display " expected: <")
    (write expected)
    (display ">")
    (newline)
    (display "   actual: <")
    (write actual)
    (display ">")
    (newline)))

(define assert
  (let ((+ +))  ;; protect from redefinition
    (lambda (test-name err-msg exp)
      (set! *total-assertions* (+ *total-assertions* 1))
      (if *test-track-progress*
          (begin
            (display "done: ")
            (display test-name)
            (newline)))
      (if exp
          #t
          (begin
            (set! *total-failures* (+ *total-failures* 1))
            (report-error err-msg)
            #f)))))


;; (define assert
;;   (lambda (test-name err-msg exp)
;;     (set! *total-assertions* (+ *total-assertions* 1))
;;     (if *test-track-progress*
;;         (begin
;;           (display "done: ")
;;           (display test-name)
;;           (newline)))
;;       (if exp
;;           #t
;;           (begin
;;             (set! *total-failures* (+ *total-failures* 1))
;;             (report-error err-msg)
;;             #f))))



(define assert-true
  (lambda (test-name exp)
    (assert test-name test-name exp)))


(assert "hoge-test" "error desuyo!" 1)
(display *total-failures*)

(define test-name "procedure + test")
(assert-true test-name (= 3 (+ 1 2)))
(assert-true test-name (= 2 (+ 4 -2)))
