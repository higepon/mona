(import (rnrs)
        (mosh test)
        (shell path))

(test-equal "/APPS/HELLO.EX5" (complement "HELLO"))

(test-results)
