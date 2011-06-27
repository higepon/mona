(import (rnrs)
        (mosh test)
        (shell path))

(test-equal "/APPS/HELLO.EX5" (complement "HELLO"))
(test-equal "/APPS/MONAGUI/FACEBOOK.EX5" (complement "FACEBOOK"))

(test-results)
