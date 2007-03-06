(define file-for-each-line (lambda (file func)
                             (with-input-from-file file
                               (lambda ()
                                 (let loop ((line (read-line)))
                                   (if (eof-object? line)
                                       '()
                                       (begin (func line) (loop (read-line)))))))))
(define p (lambda (s) (display s) (newline)))

(define register-code (lambda (class name)
                        (format #f "env->defineVariable(new Variable(~a), new ~a());" name class)))

(define header-code (lambda (class name)
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
                              )))

(define register-from-file (cut file-for-each-line <> (lambda (line)
                                                        (let ((match (#/^PROCEDURE\(([^,]+),\s*([^\)]+)\)/ line)))
                                                          (if match
                                                              (p (register-code (match 1) (match 2))))))))

(define header-from-file (cut file-for-each-line <> (lambda (line)
                                                        (let ((match (#/^PROCEDURE\(([^,]+),\s*([^\)]+)\)/ line)))
                                                          (if match
                                                              (p (header-code (match 1) (match 2))))))))


(define main (lambda (args)
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
                          ))))))

