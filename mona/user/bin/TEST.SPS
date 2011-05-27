(import (rnrs)
        (monapi))
(display "hello")
(display (monapi-message-send (monapi-name-whereis "/servers/shell") 1 2 3 4 #vu8(1 2 3)))
