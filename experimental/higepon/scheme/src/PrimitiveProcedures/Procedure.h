#ifndef __PRIMITIVE_PROCEDURE_PROCEDURE_H__
#define __PRIMITIVE_PROCEDURE_PROCEDURE_H__

#include "PrimitiveProcedure.h"


#define RETURN_BOOLEAN(condition) \
    if ((condition))              \
    {                             \
        return new True();        \
    }                             \
    else                          \
    {                             \
        return new False();       \
    }

#define ARGV(i) (as->at(i))
#define ARGC  (as->size())

#define SCHEME_CAST(o, type, to)                                                 \
    if (!o->is##type())                                                          \
    {                                                                            \
        RAISE_ERROR(o->lineno(), "%s got wrong argument", toString().c_str());   \
    }                                                                            \
    type* to = (type*)o;

#define ARGC_SHOULD_BE(n)                                                        \
    Objects* as = listOfValues(arguments, env);                                  \
    if (ARGC != n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0                            \
                    , "%s got %d arguments, but required %d"                     \
                    , toString().c_str(), ARGC, n);                              \
    }

#define PROCEDURE_BEGIN(ClassName, name)       \
class ClassName : public PrimitiveProcedure\
{\
private:\
    std::string procedureName_;                \
public:\
    ClassName()  : procedureName_(""#name"") {} \
    virtual ~ClassName() {}    \
\
    virtual std::string toString() {    return "procedure:" + procedureName_;} \
    virtual Object* eval(Environment* env) { RAISE_ERROR(lineno(), "don't eval procedure %s", procedureName_.c_str()); return NULL;} \
    virtual Object* apply(Objects* arguments, Environment* env); \
};\
Object* ClassName::apply(Objects* arguments, Environment* env)




#endif // __PRIMITIVE_PROCEDURE_PROCEDURE_H__
