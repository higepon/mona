(load "/LIBS/SCHEME/core.scm")
(load "/LIBS/SCHEME/danny.scm")
(load "/LIBS/SCHEME/shell.scm")
(load "/LIBS/SCHEME/timer.scm")
(load "/LIBS/SCHEME/conf.scm")
;(load "slib/Template.scm")
;(slib:report)

(load "/LIBS/SCHEME/mona.scm")
;(load "/LIBS/SCHEME/linux.scm")

(define *fs-root-directory* (fs-new-directory "/"))
;(fs-create-tree *fs-root-directory* "/home/taro/mona/bin")
(fs-create-tree *fs-root-directory* "/")
(define *fs-current-directory* *fs-root-directory*)

(define status 0)

