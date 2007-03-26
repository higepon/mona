; "prec.scm", dynamically extensible parser/tokenizer	-*-scheme-*-
; Copyright 1989, 1990, 1991, 1992, 1993, 1995, 1997 Aubrey Jaffer
;
;Permission to copy this software, to modify it, to redistribute it,
;to distribute modified versions, and to use it for any purpose is
;granted, subject to the following restrictions and understandings.
;
;1.  Any copy made of this software must include this copyright notice
;in full.
;
;2.  I have made no warranty or representation that the operation of
;this software will be error-free, and I am under no obligation to
;provide any services, by way of maintenance, update, or otherwise.
;
;3.  In conjunction with products arising from the use of this
;material, there shall be no use of my name in any advertising,
;promotional, or sales literature without prior written consent in
;each case.

; This file implements:
; * a Pratt style parser.
; * a tokenizer which congeals tokens according to assigned classes of
;   constituent characters.
;
; This module is a significant improvement because grammar can be
; changed dynamically from rulesets which don't need compilation.
; Theoretically, all possibilities of bad input are handled and return
; as much structure as was parsed when the error occured; The symbol
; `?' is substituted for missing input.

; References for the parser are:

; Pratt, V. R.
; Top Down Operator Precendence.
; SIGACT/SIGPLAN
; Symposium on Principles of Programming Languages,
; Boston, 1973, 41-51

; WORKING PAPER 121
; CGOL - an Alternative External Representation For LISP users
; Vaughan R. Pratt
; MIT Artificial Intelligence Lab.
; March 1976

; Mathlab Group,
; MACSYMA Reference Manual, Version Ten,
; Laboratory for Computer Science, MIT, 1983

(require 'fluid-let)
(require 'string-search)
(require 'string-port)
(require 'delay)

(define *syn-rules* #f)			;Dynamically bound
(define *prec:port* #f)			;Dynamically bound
;@
(define *syn-defs* #f)

;; keeps track of input column so we can generate useful error displays.
(define tok:column 0)
(define (tok:peek-char) (peek-char *prec:port*))
(define (tok:read-char)
  (let ((c (read-char *prec:port*)))
    (if (or (eqv? c #\newline) (eof-object? c))
	(set! tok:column 0)
	(set! tok:column (+ 1 tok:column)))
    c))
;@
(define (tok:bump-column pos . ports)
  ((lambda (thunk)
     (cond ((null? ports) (thunk))
	   (else (fluid-let ((*prec:port* (car ports))
			     (prec:token #f))
		   (thunk)))))
   (lambda ()
     (cond ((eqv? #\newline (tok:peek-char))
	    (tok:read-char)))		;to do newline
     (set! tok:column (+ tok:column pos)))))

(define (prec:warn . msgs)
  (do ((j (+ -1 tok:column) (+ -8 j)))
      ((> 8 j)
       (do ((i j (+ -1 i)))
	   ((>= 0 i)
	    (display "^ ")
	    (newline)
	    (for-each (lambda (x) (write x) (display #\space)) msgs)
	    (newline))
	 (display #\space)))
    (display slib:tab)))

;; Structure of lexical records.
(define tok:make-rec cons)
(define tok:cc car)
(define tok:sfp cdr)

(define (tok:lookup alist char)
  (if (eof-object? char)
      #f
      (let ((pair (assv char alist)))
	(and pair (cdr pair)))))
;@
(define (tok:char-group group chars chars-proc)
  (map (lambda (token)
;;;	 (let ((oldlexrec (tok:lookup *syn-defs* token)))
;;;	   (cond ((or (not oldlexrec) (eqv? (tok:cc oldlexrec) group)))
;;;		 (else (math:warn 'cc-of token 'redefined-to- group))))
	 (cons token (tok:make-rec group chars-proc)))
       (cond ((string? chars) (string->list chars))
	     ((char? chars) (list chars))
	     (else chars))))

(define (tokenize)
  (let* ((char (tok:read-char))
	 (rec (tok:lookup *syn-rules* char))
	 (proc (and rec (tok:cc rec)))
	 (clist (list char)))
    (cond
     ((not proc) char)
     ((procedure? proc)
      (do ((cl clist (begin (set-cdr! cl (list (tok:read-char))) (cdr cl))))
	  ((proc (tok:peek-char))
	   ((or (tok:sfp rec) list->string) clist))))
     ((eqv? 0 proc) (tokenize))
     (else
      (do ((cl clist (begin (set-cdr! cl (list (tok:read-char))) (cdr cl))))
	  ((not (let* ((prec (tok:lookup *syn-rules* (tok:peek-char)))
		       (cclass (and prec (tok:cc prec))))
		  (or (eqv? cclass proc)
		      (eqv? cclass (+ -1 proc)))))
	   ((tok:sfp rec) clist)))))))

;;; PREC:NUD is the null denotation (function and arguments to call when no
;;;	unclaimed tokens).
;;; PREC:LED is the left denotation (function and arguments to call when
;;;	unclaimed token is on left).
;;; PREC:LBP is the left binding power of this LED.  It is the first
;;; argument position of PREC:LED

(define (prec:nudf alist self)
  (let ((pair (assoc (cons 'nud self) alist)))
    (and pair (cdr pair))))
(define (prec:ledf alist self)
  (let ((pair (assoc (cons 'led self) alist)))
    (and pair (cdr pair))))
(define (prec:lbp alist self)
  (let ((pair (assoc (cons 'led self) alist)))
    (and pair (cadr pair))))

(define (prec:call-or-list proc . args)
  (prec:apply-or-cons proc args))
(define (prec:apply-or-cons proc args)
  (if (procedure? proc) (apply proc args) (cons (or proc '?) args)))

;;; PREC:SYMBOLFY and PREC:DE-SYMBOLFY are not exact inverses.
(define (prec:symbolfy obj)
  (cond ((symbol? obj) obj)
	((string? obj) (string->symbol obj))
	((char? obj) (string->symbol (string obj)))
	(else obj)))

(define (prec:de-symbolfy obj)
  (cond ((symbol? obj) (symbol->string obj))
	(else obj)))

;;;Calls to set up tables.
;@
(define (prec:define-grammar . synlsts)
  (set! *syn-defs* (append (apply append synlsts) *syn-defs*)))
;@
(define (prec:make-led toks . args)
  (map (lambda (tok)
	 (cons (cons 'led (prec:de-symbolfy tok))
	       args))
       (if (pair? toks) toks (list toks))))
(define (prec:make-nud toks . args)
  (map (lambda (tok)
	 (cons (cons 'nud (prec:de-symbolfy tok))
	       args))
       (if (pair? toks) toks (list toks))))

;;; Produce dynamically augmented grammars.
(define (prec:process-binds binds rules)
  (if (and #f (not (null? binds)) (eq? #t (car binds)))
      (cdr binds)
      (append binds rules)))

;;(define (prec:replace-rules) some-sort-of-magic-cookie)

;;; Here are the procedures to define high-level grammar, along with
;;; utility functions called during parsing.  The utility functions
;;; (prec:parse-*) could be incorportated into the defining commands,
;;; but tracing these functions is useful for debugging.
;@
(define (prec:delim tk)
  (prec:make-led tk 0 #f))
;@
(define (prec:nofix tk sop . binds)
  (prec:make-nud tk prec:parse-nofix sop (apply append binds)))

(define (prec:parse-nofix self sop binds)
  (set! *syn-rules* (prec:process-binds binds *syn-rules*))
  (prec:call-or-list (or sop (prec:symbolfy self))))
;@
(define (prec:prefix tk sop bp . binds)
  (prec:make-nud tk prec:parse-prefix sop bp (apply append binds)))

(define (prec:parse-prefix self sop bp binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (prec:call-or-list (or sop (prec:symbolfy self)) (prec:parse1 bp))))
;@
(define (prec:infix tk sop lbp bp . binds)
  (prec:make-led tk lbp prec:parse-infix sop bp (apply append binds)))

(define (prec:parse-infix left self lbp sop bp binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (prec:call-or-list (or sop (prec:symbolfy self)) left (prec:parse1 bp))))
;@
(define (prec:nary tk sop bp)
  (prec:make-led tk bp prec:parse-nary sop bp))

(define (prec:parse-nary left self lbp sop bp)
  (prec:apply-or-cons (or sop (prec:symbolfy self))
		      (cons left (prec:parse-list self bp))))
;@
(define (prec:postfix tk sop lbp . binds)
  (prec:make-led tk lbp prec:parse-postfix sop (apply append binds)))

(define (prec:parse-postfix left self lbp sop binds)
  (set! *syn-rules* (prec:process-binds binds *syn-rules*))
  (prec:call-or-list (or sop (prec:symbolfy self)) left))
;@
(define (prec:prestfix tk sop bp . binds)
  (prec:make-nud tk prec:parse-rest sop bp (apply append binds)))

(define (prec:parse-rest self sop bp binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (prec:apply-or-cons (or sop (prec:symbolfy self)) (prec:parse-list #f bp))))
;@
(define (prec:commentfix tk stp match . binds)
  (append
   (prec:make-nud tk prec:parse-nudcomment stp match (apply append binds))
   (prec:make-led tk 220 prec:parse-ledcomment stp match (apply append binds))))

(define (prec:parse-nudcomment self stp match binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (tok:read-through-comment stp match)
    (prec:advance)
    (cond ((prec:delim? (force prec:token)) #f)
	  (else (prec:parse1 prec:bp)))))
(define (prec:parse-ledcomment left lbp self stp match binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (tok:read-through-comment stp match)
    (prec:advance)
    left))
(define (tok:read-through-comment stp match)
  (set! match (if (char? match)
		  (string match)
		  (prec:de-symbolfy match)))
  (cond ((procedure? stp)
	 (let* ((len #f)
		(str (call-with-output-string
		      (lambda (sp)
			(set! len (find-string-from-port?
				   match *prec:port*
				   (lambda (c) (display c sp) #f)))))))
	   (stp (and len (substring str 0 (- len (string-length match)))))))
	(else (find-string-from-port? match *prec:port*))))
;@
(define (prec:matchfix tk sop sep match . binds)
  (define sep-lbp 0)
  (prec:make-nud tk prec:parse-matchfix
		 sop sep-lbp sep match
		 (apply append (prec:delim match) binds)))

(define (prec:parse-matchfix self sop sep-lbp sep match binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (cond (sop (prec:apply-or-cons
		sop (prec:parse-delimited sep sep-lbp match)))
	  ((equal? (force prec:token) match)
	   (prec:warn 'expression-missing)
	   (prec:advance)
	   '?)
	  (else (let ((ans (prec:parse1 0))) ;just parenthesized expression
		  (cond ((equal? (force prec:token) match)
			 (prec:advance))
			((prec:delim? (force prec:token))
			 (prec:warn 'mismatched-delimiter (force prec:token)
				    'not match)
			 (prec:advance))
			(else (prec:warn 'delimiter-expected--ignoring-rest
					 (force prec:token) 'expected match
					 'or-delimiter)
			      (do () ((prec:delim? (force prec:token)))
				(prec:parse1 0))))
		  ans)))))
;@
(define (prec:inmatchfix tk sop sep match lbp . binds)
  (define sep-lbp 0)
  (prec:make-led tk lbp prec:parse-inmatchfix
		 sop sep-lbp sep match
		 (apply append (prec:delim match) binds)))

(define (prec:parse-inmatchfix left self lbp sop sep-lbp sep match binds)
  (fluid-let ((*syn-rules* (prec:process-binds binds *syn-rules*)))
    (prec:apply-or-cons
     sop (cons left (prec:parse-delimited sep sep-lbp match)))))

;;;; Here is the code which actually parses.

(define prec:bp #f)			;dynamically bound
(define prec:token #f)
(define (prec:advance)
  (set! prec:token (delay (tokenize))))
(define (prec:advance-return-last)
  (let ((last (and prec:token (force prec:token))))
    (prec:advance)
    last))

(define (prec:nudcall self)
  (let ((pob (prec:nudf *syn-rules* self)))
    (cond
     (pob (let ((proc (car pob)))
	    (cond ((procedure? proc) (apply proc self (cdr pob)))
		  (proc (cons proc (cdr pob)))
		  (else '?))))
     ((char? self) (prec:warn 'extra-separator)
		   (prec:advance)
		   (prec:nudcall (force prec:token)))
     ((string? self) (string->symbol self))
     (else self))))

(define (prec:ledcall left self)
  (let* ((pob (prec:ledf *syn-rules* self)))
    (apply (cadr pob) left self (cdr pob))))

;;; PREC:PARSE1 is the heart.
(define (prec:parse1 bp)
  (fluid-let ((prec:bp bp))
    (do ((left (prec:nudcall (prec:advance-return-last))
	       (prec:ledcall left (prec:advance-return-last))))
	((or (>= bp 200)		;to avoid unneccesary lookahead
	     (>= bp (or (prec:lbp *syn-rules* (force prec:token)) 0))
	     (not left))
	 left))))

(define (prec:delim? token)
  (or (eof-object? token) (<= (or (prec:lbp *syn-rules* token) 220) 0)))

(define (prec:parse-list sep bp)
  (cond ((prec:delim? (force prec:token))
	 (prec:warn 'expression-missing)
	 '(?))
	(else
	 (let ((f (prec:parse1 bp)))
	   (cons f (cond ((equal? (force prec:token) sep)
			  (prec:advance)
			  (cond ((equal? (force prec:token) sep)
				 (prec:warn 'expression-missing)
				 (prec:advance)
				 (cons '? (prec:parse-list sep bp)))
				((prec:delim? (force prec:token))
				 (prec:warn 'expression-missing)
				 '(?))
				(else (prec:parse-list sep bp))))
			 ((prec:delim? (force prec:token)) '())
			 ((not sep) (prec:parse-list sep bp))
			 ((prec:delim? sep) (prec:warn 'separator-missing)
					    (prec:parse-list sep bp))
			 (else '())))))))

(define (prec:parse-delimited sep bp delim)
  (cond ((equal? (force prec:token) sep)
	 (prec:warn 'expression-missing)
	 (prec:advance)
	 (cons '? (prec:parse-delimited sep delim)))
	((prec:delim? (force prec:token))
	 (if (not (equal? (force prec:token) delim))
	     (prec:warn 'mismatched-delimiter (force prec:token)
			'expected delim))
	 (if (not sep) (prec:warn 'expression-missing))
	 (prec:advance)
	 (if sep '() '(?)))
	(else (let ((ans (prec:parse-list sep bp)))
		(cond ((equal? (force prec:token) delim))
		      ((prec:delim? (force prec:token))
		       (prec:warn 'mismatched-delimiter (force prec:token)
				  'expecting delim))
		      (else (prec:warn 'delimiter-expected--ignoring-rest
				       (force prec:token) '...)
			    (do () ((prec:delim? (force prec:token)))
			      (prec:parse1 bp))))
		(prec:advance)
		ans))))
;@
(define (prec:parse grammar delim . port)
  (set! delim (prec:de-symbolfy delim))
  (fluid-let ((*syn-rules* (append (prec:delim delim) grammar))
	      (*prec:port* (if (null? port) (current-input-port) (car port)))
	      (prec:token prec:token))
    (prec:advance)			; setup prec:token with first token
    (cond ((eof-object? (force prec:token)) (force prec:token))
	  ((equal? (force prec:token) delim) #f)
	  (else
	   (let ((ans (prec:parse1 0)))
	     (cond ((eof-object? (force prec:token)))
		   ((equal? (force prec:token) delim))
		   (else (prec:warn 'delimiter-expected--ignoring-rest
				    (force prec:token) 'not delim)
			 (do () ((or (equal? (force prec:token) delim)
				     (eof-object? (force prec:token))))
			   (prec:advance))))
	     ans)))))
;@
(define tok:decimal-digits "0123456789")
(define tok:upper-case "ABCDEFGHIJKLMNOPQRSTUVWXYZ")
(define tok:lower-case "abcdefghijklmnopqrstuvwxyz")
(define tok:whitespaces
  (do ((i (+ -1 (min 256 char-code-limit)) (+ -1 i))
       (ws "" (if (char-whitespace? (integer->char i))
		  (string-append ws (string (integer->char i)))
		  ws)))
      ((negative? i) ws)))

;;;;The parse tables.
;;; Definitions accumulate in top-level variable *SYN-DEFS*.
(set! *syn-defs* '())			;Make sure *SYN-DEFS* is empty.

;;; Ignore Whitespace characters.
(prec:define-grammar (tok:char-group 0 tok:whitespaces #f))

;;; On MS-DOS systems, <ctrl>-Z (26) needs to be ignored in order to
;;; avoid problems at end of files.
(case (software-type)
  ((ms-dos)
   (if (not (char-whitespace? (integer->char 26)))
       (prec:define-grammar (tok:char-group 0 (integer->char 26) #f))
       )))

;;;@ Save these convenient definitions.
(define *syn-ignore-whitespace* *syn-defs*)
(set! *syn-defs* '())

;;(begin (trace-all "prec.scm") (set! *qp-width* 333))
;;(pretty-print (grammar-read-tab (get-grammar 'standard)))
;;(prec:trace)
