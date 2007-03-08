;; useful thx! rui
(define (file-for-each file proc reader)
  (call-with-input-file file
      (lambda (in)
        (port-for-each proc (cut reader in)))))

;; (file-for-each "/home/taro/higepon/diary.txt" print read-line)

(define (register-code class name)
  (format #f "env->defineVariable(new Variable(~a), new ~a());" name class))

(define (header-code class name)
  (format #f (string-append
              "class ~a : public PrimitiveProcedure\n"
              "{\n"
              "private:\n"
              "    ::util::String procedureName_;\n"
              "public:\n"
              "    ~a()  : procedureName_(~a) {}\n"
              "    virtual ~~~a() {}\n"
              "\n"
              "    virtual ::util::String toString() {    return \"procedure:\"~a\;}\n"
              "    virtual ::util::String typeString() const { return ~a; }\n"
              "    virtual Object* eval(Environment* env)\n"
              "    {\n"
              "        return this;\n"
              "    }\n"
              "    virtual Object* apply(Objects* arguments, Environment* env);\n"
              "};\n")
          class
          class
          name
          class
          name
          name
          ))

(define (register-from-file file)
  (define (print-if-match line)
    (rxmatch-cond ((#/^PROCEDURE\(([^,]+),\s*([^\)]+)\)/ line)
                  (#f class name)
                  (print (register-code class name)))))
  (file-for-each file print-if-match read-line))

(define (header-from-file file)
  (define (print-if-match line)
    (rxmatch-cond ((#/^PROCEDURE\(([^,]+),\s*([^\)]+)\)/ line)
                  (#f class name)
                  (print (header-code class name)))))
  (file-for-each file print-if-match read-line))

(define (main args)
  (let ((option (cadr args)) (list (cddr args)))
    (cond ((string=? option "--register") (for-each register-from-file list))
          ((string=? option "--header")
           (begin
             (display (string-append
                       "#ifndef __PRIMITIVE_PROCEDURES_H__\n"
                       "#define __PRIMITIVE_PROCEDURES_H__\n"
                       "#include \"PrimitiveProcedure.h\"\n"
                       "namespace monash { \n"))
             (for-each header-from-file list)
             (display "};\n")
             (display "#endif\n")
             ))))
  0)

