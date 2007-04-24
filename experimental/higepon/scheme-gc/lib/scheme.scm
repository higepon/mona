(load "./lib/core.scm")
(load "./lib/danny.scm")
(load "./lib/shell.scm")
;(load "slib/Template.scm")
;(slib:report)

;(load "./lib/mona.scm")
(load "./lib/linux.scm")

(define *fs-root-directory* (fs-new-directory "/"))
(fs-create-tree *fs-root-directory* "/home/taro/mona/bin")

(define *fs-current-directory* *fs-root-directory*)
