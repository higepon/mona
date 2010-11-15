(import (rnrs) (mosh file))

(display (file->string (cadr (command-line))))
(newline)