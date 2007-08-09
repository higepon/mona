;;;"cring.scm" Extend Scheme numerics to any commutative ring.
;Copyright (C) 1997, 1998, 2001 Aubrey Jaffer
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

(require 'common-list-functions)
(require 'relational-database)
(require 'databases)
(require 'sort)

(define number^ expt)
(define number* *)
(define number+ +)
(define number- -)
(define number/ /)
(define number0? zero?)
(define (zero? x) (and (number? x) (number0? x)))
;;(define (sign x) (if (positive? x) 1 (if (negative? x) -1 0)))

(define cring:db (create-database #f 'alist-table))
;@
(define (make-ruleset . rules)
  (define name #f)
  (cond ((and (not (null? rules)) (symbol? (car rules)))
	 (set! name (car rules))
	 (set! rules (cdr rules)))
	(else (set! name (gentemp))))
  (define-tables cring:db
    (list name
	  '((op symbol)
	    (sub-op1 symbol)
	    (sub-op2 symbol))
	  '((reduction expression))
	  rules))
  (let ((table ((cring:db 'open-table) name #t)))
    (and table
	 (list (table 'get 'reduction)
	       (table 'row:update)
	       table))))
;@
(define *ruleset* (make-ruleset 'default))
(define (cring:define-rule . args)
  (if *ruleset*
      ((cadr *ruleset*) args)
      (slib:warn "No ruleset in *ruleset*")))
;@
(define (combined-rulesets . rulesets)
  (define name #f)
  (cond ((symbol? (car rulesets))
	 (set! name (car rulesets))
	 (set! rulesets (cdr rulesets)))
	(else (set! name (gentemp))))
  (apply make-ruleset name
	 (apply append
		(map (lambda (ruleset) (((caddr ruleset) 'row:retrieve*)))
		     rulesets))))

;;; Distribute * over + (and -)
;@
(define distribute*
  (make-ruleset
   'distribute*
   `(* + identity
       ,(lambda (exp1 exp2)
	  ;;(print 'distributing '* '+ exp1 exp2 '==>)
	  (apply + (map (lambda (trm) (* trm exp2)) (cdr exp1)))))
   `(* - identity
       ,(lambda (exp1 exp2)
	  ;;(print 'distributing '* '- exp1 exp2 '==>)
	  (apply - (map (lambda (trm) (* trm exp2)) (cdr exp1)))))))

;;; Distribute / over + (and -)
;@
(define distribute/
  (make-ruleset
   'distribute/
   `(/ + identity
       ,(lambda (exp1 exp2)
	  ;;(print 'distributing '/ '+ exp1 exp2 '==>)
	  (apply + (map (lambda (trm) (/ trm exp2)) (cdr exp1)))))
   `(/ - identity
       ,(lambda (exp1 exp2)
	  ;;(print 'distributing '/ '- exp1 exp2 '==>)
	  (apply - (map (lambda (trm) (/ trm exp2)) (cdr exp1)))))))

(define (symbol-alpha? sym)
  (char-alphabetic? (string-ref (symbol->string sym) 0)))
(define (expression-< x y)
  (cond ((and (number? x) (number? y)) (> x y))	;want negatives last
	((number? x) #t)
	((number? y) #f)
	((and (symbol? x) (symbol? y))
	 (cond ((eqv? (symbol-alpha? x) (symbol-alpha? y))
		(string<? (symbol->string x) (symbol->string y)))
	       (else (symbol-alpha? x))))
	((symbol? x) #t)
	((symbol? y) #f)
	((null? x) #t)
	((null? y) #f)
	((expression-< (car x) (car y)) #t)
	((expression-< (car y) (car x)) #f)
	(else (expression-< (cdr x) (cdr y)))))
(define (expression-sort seq) (sort! seq expression-<))

(define is-term-op? (lambda (term op) (and (pair? term) (eq? op (car term)))))

;; To convert to CR internal form, NUMBER-op all the `numbers' in the
;; argument list and remove them from the argument list.  Collect the
;; remaining arguments into equivalence classes, keeping track of the
;; number of arguments in each class.  The returned list is thus:
;; (<numeric> (<expression1> . <exp1>) ...)

;;; Converts * argument list to CR internal form
(define (cr*-args->fcts args)
  ;;(print (cons 'cr*-args->fcts args) '==>)
  (let loop ((args args) (pow 1) (nums 1) (arg.exps '()))
    ;;(print (list 'loop args pow nums denoms arg.exps) '==>)
    (cond ((null? args) (cons nums arg.exps))
	  ((number? (car args))
	   (let ((num^pow (number^ (car args) (abs pow))))
	     (if (negative? pow)
		 (loop (cdr args) pow (number/ (number* num^pow nums))
		       arg.exps)
		 (loop (cdr args) pow (number* num^pow nums) arg.exps))))
	  ;; Associative Rule
	  ((is-term-op? (car args) '*) (loop (append (cdar args) (cdr args))
					     pow nums arg.exps))
	  ;; Do singlet -
	  ((and (is-term-op? (car args) '-) (= 2 (length (car args))))
	   ;;(print 'got-here (car args))
	   (set! arg.exps (loop (cdar args) pow (number- nums) arg.exps))
	   (loop (cdr args) pow
		 (car arg.exps)
		 (cdr arg.exps)))
	  ((and (is-term-op? (car args) '/) (= 2 (length (car args))))
	   ;; Do singlet /
	   ;;(print 'got-here=cr+ (car args))
	   (set! arg.exps (loop (cdar args) (number- pow) nums arg.exps))
	   (loop (cdr args) pow
		 (car arg.exps)
		 (cdr arg.exps)))
	  ((is-term-op? (car args) '/)
	   ;; Do multi-arg /
	   ;;(print 'doing '/ (cddar args) (number- pow))
	   (set! arg.exps
		 (loop (cddar args) (number- pow) nums arg.exps))
	   ;;(print 'finishing '/ (cons (cadar args) (cdr args)) pow)
	   (loop (cons (cadar args) (cdr args))
		 pow
		 (car arg.exps)
		 (cdr arg.exps)))
	  ;; Pull out numeric exponents as powers
	  ((and (is-term-op? (car args) '^)
		(= 3 (length (car args)))
		(number? (caddar args)))
	   (set! arg.exps (loop (list (cadar args))
				(number* pow (caddar args))
				nums
				arg.exps))
	   (loop (cdr args) pow (car arg.exps) (cdr arg.exps)))
	  ;; combine with same terms
	  ((assoc (car args) arg.exps)
	   => (lambda (pair) (set-cdr! pair (number+ pow (cdr pair)))
		      (loop (cdr args) pow nums arg.exps)))
	  ;; Add new term to arg.exps
	  (else (loop (cdr args) pow nums
		      (cons (cons (car args) pow) arg.exps))))))

;;; Converts + argument list to CR internal form
(define (cr+-args->trms args)
  (let loop ((args args) (cof 1) (numbers 0) (arg.exps '()))
    (cond ((null? args) (cons numbers arg.exps))
	  ((number? (car args))
	   (loop (cdr args)
		 cof
		 (number+ (number* (car args) cof) numbers)
		 arg.exps))
	  ;; Associative Rule
	  ((is-term-op? (car args) '+) (loop (append (cdar args) (cdr args))
					     cof
					     numbers
					     arg.exps))
	  ;; Idempotent singlet *
	  ((and (is-term-op? (car args) '*) (= 2 (length (car args))))
	   (loop (cons (cadar args) (cdr args))
		 cof
		 numbers
		 arg.exps))
	  ((and (is-term-op? (car args) '-) (= 2 (length (car args))))
	   ;; Do singlet -
	   (set! arg.exps (loop (cdar args) (number- cof) numbers arg.exps))
	   (loop (cdr args) cof (car arg.exps) (cdr arg.exps)))
	  ;; Pull out numeric factors as coefficients
	  ((and (is-term-op? (car args) '*) (some number? (cdar args)))
	   ;;(print 'got-here (car args) '=> (cons '* (remove-if number? (cdar args))))
	   (set! arg.exps
		 (loop (list (cons '* (remove-if number? (cdar args))))
		       (apply number* cof (remove-if-not number? (cdar args)))
		       numbers
		       arg.exps))
	   (loop (cdr args) cof (car arg.exps) (cdr arg.exps)))
	  ((is-term-op? (car args) '-)
	   ;; Do multi-arg -
	   (set! arg.exps (loop (cddar args) (number- cof) numbers arg.exps))
	   (loop (cons (cadar args) (cdr args))
		 cof
		 (car arg.exps)
		 (cdr arg.exps)))
	  ;; combine with same terms
	  ((assoc (car args) arg.exps)
	   => (lambda (pair) (set-cdr! pair (number+ cof (cdr pair)))
		      (loop (cdr args) cof numbers arg.exps)))
	  ;; Add new term to arg.exps
	  (else (loop (cdr args) cof numbers
		      (cons (cons (car args) cof) arg.exps))))))

;;; Converts + or * internal form to Scheme expression
(define (cr-terms->form op ident inv-op higher-op res.cofs)
  (define (negative-cof? fct.cof)
    (negative? (cdr fct.cof)))
  (define (finish exprs)
    (if (null? exprs) ident
	(if (null? (cdr exprs))
	    (car exprs)
	    (cons op exprs))))
  (define (do-terms sign fct.cofs)
    (expression-sort
     (map (lambda (fct.cof)
	    (define cof (number* sign (cdr fct.cof)))
	    (cond ((eqv? 1 cof) (car fct.cof))
		  ((number? (car fct.cof)) (number* cof (car fct.cof)))
		  ((is-term-op? (car fct.cof) higher-op)
		   (if (eq? higher-op '^)
		       (list '^ (cadar fct.cof) (* cof (caddar fct.cof)))
		       (cons higher-op (cons cof (cdar fct.cof)))))
		  ((eqv? -1 cof) (list inv-op (car fct.cof)))
		  (else (list higher-op (car fct.cof) cof))))
	  fct.cofs)))
  (let* ((all.cofs (remove-if (lambda (fct.cof)
				(or (zero? (cdr fct.cof))
				    (eqv? ident (car fct.cof))))
			      res.cofs))
	 (cofs (map cdr all.cofs))
	 (some-positive? (some positive? cofs)))
    ;;(print op 'positive? some-positive? 'negative? (some negative? cofs) all.cofs)
    (cond ((and some-positive? (some negative? cofs))
	   (append (list inv-op
			 (finish (do-terms
				  1 (remove-if negative-cof? all.cofs))))
		   (do-terms -1 (remove-if-not negative-cof? all.cofs))))
	  (some-positive? (finish (do-terms 1 all.cofs)))
	  ((not (some negative? cofs)) ident)
	  (else (list inv-op (finish (do-terms -1 all.cofs)))))))

(define (* . args)
  (cond
   ((null? args) 1)
   ;;This next line is commented out so ^ will collapse numerical expressions.
   ;;((null? (cdr args)) (car args))
   (else
    (let ((in (cr*-args->fcts args)))
      (cond
       ((zero? (car in)) 0)
       (else
	(if (null? (cdr in))
	    (set-cdr! in (list (cons 1 1))))
	(let* ((num #f)
	       (ans (cr-terms->form
		     '* 1 '/ '^
		     (apply
		      (lambda (numeric red.cofs res.cofs)
			(set! num numeric)
			(append
			 ;;(list (cons (abs numeric) 1))
			 red.cofs
			 res.cofs))
		      (cr1 '* number* '^ '/ (car in) (cdr in))))))
	  (cond ((number0? (+ -1 num)) ans)
		((number? ans) (number* num ans))
		((number0? (+ 1 num))
		 (if (and (list? ans) (= 2 (length ans)) (eq? '- (car ans)))
		     (cadr ans)
		     (list '- ans)))
		((not (pair? ans)) (list '* num ans))
		(else
		 (case (car ans)
		   ((*) (append (list '* num) (cdr ans)))
		   ((+) (apply + (map (lambda (mon) (* num mon)) (cdr ans))))
		   ((-) (apply - (map (lambda (mon) (* num mon)) (cdr ans))))
		   (else (list '* num ans))))))))))))

(define (+ . args)
  (cond ((null? args) 0)
	;;((null? (cdr args)) (car args))
	(else
	 (let ((in (cr+-args->trms args)))
	   (if (null? (cdr in))
	       (car in)
	       (cr-terms->form
		'+ 0 '- '*
		(apply (lambda (numeric red.cofs res.cofs)
			 (append
			  (list (if (and (number? numeric)
					 (negative? numeric))
				    (cons (abs numeric) -1)
				    (cons numeric 1)))
			  red.cofs
			  res.cofs))
		       (cr1 '+ number+ '* '- (car in) (cdr in)))))))))

(define (- arg1 . args)
  (if (null? args)
      (if (number? arg1) (number- arg1)
	  (* -1 arg1)			;(list '- arg1)
	  )
      (+ arg1 (* -1 (apply + args)))))

;;(print `(/ ,arg1 ,@args) '=> )
(define (/ arg1 . args)
  (if (null? args)
      (^ arg1 -1)
      (* arg1 (^ (apply * args) -1))))

(define (^ arg1 arg2)
  (cond ((and (number? arg2) (integer? arg2))
	 (* (list '^ arg1 arg2)))
	(else (list '^ arg1 arg2))))

;; TRY-EACH-PAIR-ONCE algorithm.  I think this does the minimum
;; number of rule lookups given no information about how to sort
;; terms.

;; Pick equivalence classes one at a time and move them into the
;; result set of equivalence classes by searching for rules to
;; multiply an element of the chosen class by itself (if multiple) and
;; the element of each class already in the result group.  Each
;; (multiplicative) term resulting from rule application would be put
;; in the result class, if that class exists; or put in an argument
;; class if not.

(define (cr1 op number-op hop inv-op numeric in)
  (define red.pows '())
  (define res.pows '())
  (define (cring:apply-rule->terms exp1 exp2) ;(display op)
    (let ((ans (cring:apply-rule op exp1 exp2)))
      (cond ((not ans) #f)
	    ((number? ans) (list ans))
	    (else (list (cons ans 1))))))
  (define (cring:apply-inv-rule->terms exp1 exp2) ;(display inv-op)
    (let ((ans (cring:apply-rule inv-op exp1 exp2)))
      (cond ((not ans) #f)
	    ((number? ans) (list ans))
	    (else (list (cons ans 1))))))
  (let loop.arg.pow.s ((arg (caar in)) (pow (cdar in)) (arg.pows (cdr in)))
    (define (arg-loop arg.pows)
      (cond ((not (null? arg.pows))
	     (loop.arg.pow.s (caar arg.pows) (cdar arg.pows) (cdr arg.pows)))
	    (else (list numeric red.pows res.pows)))) ; Actually return!
    (define (merge-res tmp.pows multiplicity)
      (cond ((null? tmp.pows))
	    ((number? (car tmp.pows))
	     (do ((m (number+ -1 (abs multiplicity)) (number+ -1 m))
		  (n numeric (number-op n (abs (car tmp.pows)))))
		 ((negative? m) (set! numeric n)))
	     (merge-res (cdr tmp.pows) multiplicity))
	    ((or (assoc (car tmp.pows) res.pows)
		 (assoc (car tmp.pows) arg.pows))
	     => (lambda (pair)
		  (set-cdr! pair (number+
				  pow (number-op multiplicity (cdar tmp.pows))))
		  (merge-res (cdr tmp.pows) multiplicity)))
	    ((assoc (car tmp.pows) red.pows)
	     => (lambda (pair)
		  (set! arg.pows
			(cons (cons (caar tmp.pows)
				    (number+
				     (cdr pair)
				     (number* multiplicity (cdar tmp.pows))))
			      arg.pows))
		  (set-cdr! pair 0)
		  (merge-res (cdr tmp.pows) multiplicity)))
	    (else (set! arg.pows
			(cons (cons (caar tmp.pows)
				    (number* multiplicity (cdar tmp.pows)))
			      arg.pows))
		  (merge-res (cdr tmp.pows) multiplicity))))
    (define (try-fct.pow fct.pow)
      ;;(print 'try-fct.pow fct.pow op 'arg arg 'pow pow)
      (cond ((or (zero? (cdr fct.pow)) (number? (car fct.pow))) #f)
	    ((not (and (number? pow) (number? (cdr fct.pow))
		       (integer? pow)	;(integer? (cdr fct.pow))
		       ))
	     #f)
	    ;;((zero? pow) (slib:error "Don't try exp-0 terms") #f)
	    ;;((or (number? arg) (number? (car fct.pow)))
	    ;; (slib:error 'found-number arg fct.pow) #f)
	    ((and (positive? pow) (positive? (cdr fct.pow))
		  (or (cring:apply-rule->terms arg (car fct.pow))
		      (cring:apply-rule->terms (car fct.pow) arg)))
	     => (lambda (terms)
		  ;;(print op op terms)
		  (let ((multiplicity (min pow (cdr fct.pow))))
		    (set-cdr! fct.pow (number- (cdr fct.pow) multiplicity))
		    (set! pow (number- pow multiplicity))
		    (merge-res terms multiplicity))))
	    ((and (negative? pow) (negative? (cdr fct.pow))
		  (or (cring:apply-rule->terms arg (car fct.pow))
		      (cring:apply-rule->terms (car fct.pow) arg)))
	     => (lambda (terms)
		  ;;(print inv-op inv-op terms)
		  (let ((multiplicity (max pow (cdr fct.pow))))
		    (set-cdr! fct.pow (number+ (cdr fct.pow) multiplicity))
		    (set! pow (number+ pow multiplicity))
		    (merge-res terms multiplicity))))
	    ((and (positive? pow) (negative? (cdr fct.pow))
		  (cring:apply-inv-rule->terms arg (car fct.pow)))
	     => (lambda (terms)
		  ;;(print op inv-op terms)
		  (let ((multiplicity (min pow (number- (cdr fct.pow)))))
		    (set-cdr! fct.pow (number+ (cdr fct.pow) multiplicity))
		    (set! pow (number- pow multiplicity))
		    (merge-res terms multiplicity))))
	    ((and (negative? pow) (positive? (cdr fct.pow))
		  (cring:apply-inv-rule->terms (car fct.pow) arg))
	     => (lambda (terms)
		  ;;(print inv-op op terms)
		  (let ((multiplicity (max (number- pow) (cdr fct.pow))))
		    (set-cdr! fct.pow (number- (cdr fct.pow) multiplicity))
		    (set! pow (number+ pow multiplicity))
		    (merge-res terms multiplicity))))
	    (else #f)))
    ;;(print op numeric 'arg arg 'pow pow 'arg.pows arg.pows 'red.pows red.pows 'res.pows res.pows)
    ;;(trace arg-loop cring:apply-rule->terms merge-res try-fct.pow) (set! *qp-width* 333)
    (cond ((or (zero? pow) (eqv? 1 arg)) ;(number? arg) arg seems to always be 1
	   (arg-loop arg.pows))
	  ((assoc arg res.pows) => (lambda (pair)
				     (set-cdr! pair (number+ pow (cdr pair)))
				     (arg-loop arg.pows)))
	  ((and (> (abs pow) 1) (cring:apply-rule->terms arg arg))
	   => (lambda (terms)
		(merge-res terms (quotient pow 2))
		(if (odd? pow)
		    (loop.arg.pow.s arg 1 arg.pows)
		    (arg-loop arg.pows))))
	  ((or (some try-fct.pow res.pows) (some try-fct.pow arg.pows))
	   (loop.arg.pow.s arg pow arg.pows))
	  (else (set! res.pows (cons (cons arg pow) res.pows))
		(arg-loop arg.pows)))))

(define (cring:try-rule op sop1 sop2 exp1 exp2)
  (and *ruleset*
       (let ((rule ((car *ruleset*) op sop1 sop2)))
	 (and rule (rule exp1 exp2)))))

(define (cring:apply-rule op exp1 exp2)
  (and (pair? exp1)
       (or (and (pair? exp2)
		(cring:try-rule op (car exp1) (car exp2) exp1 exp2))
	   (cring:try-rule op (car exp1) 'identity exp1 exp2))))

;;(begin (trace cr-terms->form) (set! *qp-width* 333))
