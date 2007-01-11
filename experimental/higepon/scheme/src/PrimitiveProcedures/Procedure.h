#ifndef __PRIMITIVE_PROCEDURE_PROCEDURE_H__
#define __PRIMITIVE_PROCEDURE_PROCEDURE_H__

#include "PrimitiveProcedure.h"

#define PROCEDURE_DECLARE(ClassName)\
namespace monash {\
class ClassName : public PrimitiveProcedure\
{\
public:\
    ClassName();\
    virtual ~ClassName();\
\
    virtual std::string toString();\
    virtual Object* eval(Environment* env);\
    virtual Object* apply(Objects* arguments, Environment* env);\
};\
}; // namespace monash

#define PROCEDURE_BODY(ClassName, arguments, env) \
ClassName::ClassName()\
{\
}\
\
ClassName::~ClassName()\
{\
}\
\
std::string ClassName::toString()\
{\
    return "procedure:" ""#ClassName"";\
}\
\
Object* ClassName::eval(Environment* env)\
{\
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());\
    return NULL;\
}\
\
Object* ClassName::apply(Objects* arguments, Environment* env)

#endif // __PRIMITIVE_PROCEDURE_PROCEDURE_H__
