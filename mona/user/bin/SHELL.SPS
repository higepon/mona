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

(define (server)
  (receive (from header handle arg2 arg3 str) (monapi-message-receive)
   (let1 stream (monapi-make-stream handle)
   (cond
    [(= header MSG_TEXT)
      (let ([command-line (null-terminated-utf8->string str)])
        (match (string-split command-line #\space)
         [("ls" dir)
           (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "ls failed ~a" dir)))]]
           (ls stream dir))
           (monapi-message-reply from header MSG_OK)
           (server)]
         [("cat" path)
           (guard [ex [#t (write-string stream (format "cat failed (~s):~a\n" path (condition-printer ex)))]]
             (cat stream path))
           (monapi-message-reply from header MSG_OK)
           (server)]

         [("ps")
           (ps stream)
           (monapi-message-reply from header MSG_OK)
           (server)]
         [("test-all")
           (monapi-message-reply from header MSG_OK)
           (call-process+ '("MOSH" "/USER/BIN/TEST-ALL.SPS") stream)
           (server)]
         [("kill" id)
           (monapi-stream-write stream (string->utf8 (format "~a\n" (if (process-terminate! (string->number id))
                                                                        "process killed"
                                                                        "can't kill the process"))))
           (monapi-message-reply from header MSG_OK)
           (server)]
         [("restart") (monapi-message-reply from header MSG_OK)]
         [x
           (cond
             [(string=? (last x) "!")
               (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "call-process failed : ~a" x)))]]
                (call-process+ (drop-right x 1) stream))
               (monapi-message-reply from header MSG_OK)]
             [else
               (guard [ex [#t (monapi-stream-write stream (string->utf8 (format "start-process failed : ~a" x)))]]
                  (start-process+ x stream))
               (monapi-message-reply from header MSG_OK)])
            (server)
          ]))]
    [else
      (server)]))))

(define (main)
  (monapi-name-add! "/servers/shell")
  (cond
    [(= 2 (length (command-line)))
      (display "shell server restarted\n")]
    [else
;      (call-process "/APPS/MOSH.APP/MOSH.EXE /USER/TEST/TEST-ALL.SPS")
      (start-process "/SERVERS/GUI.EX5 /APPS/MONAFRMS.INI")])
  (server))

(main)
