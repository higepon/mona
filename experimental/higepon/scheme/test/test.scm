(load "./test/scheme.scm")
(load "./test/unittest.scm")
(display "hoge")
(newline)
(display "hoge")

(define a 1234)

(set! a (+ a 1))

(display a)


(define *test-track-progress* #f)  ;; for locationg SEGV point
(define *total-testsuites* 1)  ;; TODO: introduce test suites and defaults to 0
(define *total-testcases* 1)   ;; TODO: introduce testcase and defaults to 0
(define *total-tests* 1)       ;; TODO: introduce test group and defaults to 0
(define *total-failures*  0)
(define *total-assertions* 0)
(define *total-errors* 0) ;; TODO: recover unintended error and increment this
(define test-filename "unspecified")

(define total-report
  (lambda ()
    (let ((header (if (zero? *total-failures*)
                      "OK: "
                      "FAILED: "))
          (total-successes (- *total-assertions* *total-failures*)))
      (for-each display
                (list
                 header
                 *total-tests*      " tests, "
                 *total-assertions* " assertions, "
                 total-successes    " successes, "
                 *total-failures*   " failures, "
                 *total-errors*     " errors"))
      (newline))))


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
(for-each display (list 1 2 3 4 5))

(assert "hoge-test" "error desuyo!" 1)
(display *total-failures*)

(define test-name "procedure +")
(assert-true test-name (= 3 (+ 1 2)))
(assert-true test-name (= 2 (+ 4 -2)))
(assert-true test-name (= 11 (+ 4 2 5)))

(define test-name "procedure -")
(assert-true test-name (= 2 (- 3 1)))
(assert-true test-name (= 0 (- 3 1 2)))
(assert-true test-name (= -3 (- 3 1 2 3)))
(assert-true test-name (= -3 (- 3)))

(define test-name "procedure /")
(assert-true test-name (= 2 (/ 4 2)))
(assert-true test-name (= 2 (/ 8 2 2)))
(assert-true test-name (= 1 (/ 54 2 9 3)))

(define test-name "procedure *")
(assert-true test-name (= 2 (* 1 2)))
(assert-true test-name (= -8 (* 4 -2)))
(assert-true test-name (= 40 (* 4 2 5)))



(total-report)


(display (string-append "hige" "is" "miracle"))

