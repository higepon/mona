(import (rnrs)
        (monapi)
        (srfi :8)
        (mosh)
        (match)
        (mosh control)
        (mosh file)
        (shorten)
        (shell)
        (mosh process)
        (only (srfi :1) last drop-right)
        (only (srfi :13) string-contains string-join)
        (only (mosh ffi) null-terminated-utf8->string string->utf8z))

;; N.B. Don't output to standard in/out.

(define (write-string stream s)
  (monapi-stream-write stream (string->utf8 s)))

(monapi-name-add! "/servers/shell")

(call-process "/APPS/MOSH.APP/MOSH.EXE /USER/TEST/TEST-ALL.SPS")

(start-process "/SERVERS/GUI.EX5 /APPS/MONAFRMS.INI")

(let loop ()
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (let1 stream (monapi-make-stream handle)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "ls failed ~a" dir)))]]
           (ls stream dir))
           (monapi-message-reply from header MSG_OK)]
         [("ps")
           (ps stream)
           (monapi-message-reply from header MSG_OK)]
         [("test-all")
           (monapi-message-reply from header MSG_OK)
           (call-process+ '("MOSH" "/USER/BIN/TEST-ALL.SPS") stream)
         ]
         [("kill" id)
           (monapi-stream-write stream (string->utf8 (format "~a\n" (if (process-terminate! (string->number id))
                                                                        "process killed"
                                                                        "can't kill the process"))))
           (monapi-message-reply from header MSG_OK)]
         [x
           (cond
             [(string=? (last x) "&")
               (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "start-process failed : ~a" x)))]]
                 (start-process+ (drop-right x 1) stream))
               (monapi-message-reply from header MSG_OK)]
             [else
               (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "call-process failed : ~a" x)))]]
                 (call-process+ x stream))
               (monapi-message-reply from header MSG_OK)])
          ]))
      (loop)]
    [(= header MSG_NAME) (loop)]
    [else
      (loop)]))))
