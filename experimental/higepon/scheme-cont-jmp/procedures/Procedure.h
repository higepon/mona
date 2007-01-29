#ifndef __PRIMITIVE_PROCEDURE_PROCEDURE_H__
#define __PRIMITIVE_PROCEDURE_PROCEDURE_H__

#include "PrimitiveProcedure.h"
#include "Kernel.h"
#include "scheme.h"

namespace monash {
    typedef std::vector< std::pair<Variable*, Object*> > DefaultProcedures;
};

extern monash::DefaultProcedures procedures;

#define ARGV(i) (as->at(i))
#define ARGC    (as->size())

#define RETURN_BOOLEAN(condition) \
    if ((condition))              \
    {                             \
        return new True();        \
    }                             \
    else                          \
    {                             \
        return new False();       \
    }

#define CAST(o, type, to)                                                        \
    if (!o->is##type())                                                          \
    {                                                                            \
        RAISE_ERROR(o->lineno(), "%s got wrong argument", toString().c_str());   \
    }                                                                            \
    type* to = (type*)o;

#define CAST_RETURN_FALSE(o, type, to)                                           \
    if (!o->is##type())                                                          \
    {                                                                            \
        return new False();                                                      \
    }                                                                            \
    type* to = (type*)o;

#define ARGC_SHOULD_BE(n)                                                        \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC != n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0                            \
                    , "%s got %d argument(s), but required %d"                   \
                    , toString().c_str(), ARGC, n);                              \
    }

#define ARGC_SHOULD_BE_GT(n)                                                     \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                             \
                    , "%s got %d argument(s), but required at least %d"          \
                    , toString().c_str(), ARGC, n + 1);                          \
    }

#define ARGC_SHOULD_BE_GT_RETURN_FALSE(n)                                        \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        return new False();                                                      \
    }


#define ARGC_SHOULD_BE_BETWEEN(m, n)                                                         \
    Objects* as = Kernel::listOfValues(arguments, env);                                      \
    if (ARGC < m || ARGC > n)                                                                \
    {                                                                                        \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                                         \
                    , "%s got %d argument(s), but required between %d and %d arguments."     \
                    , toString().c_str(), ARGC, m, n);                                       \
    }


#define PROCEDURE(ClassName, name)                                               \
class ClassName : public PrimitiveProcedure                                      \
{                                                                                \
private:                                                                         \
    std::string procedureName_;                                                  \
public:                                                                          \
    ClassName()  : procedureName_(name) {}                                       \
    virtual ~ClassName() {}                                                      \
                                                                                 \
    virtual std::string toString() {    return "procedure:" name;}               \
    virtual Object* eval(Environment* env)                                       \
    {                                                                            \
        RAISE_ERROR(lineno(), "don't eval procedure " name);                     \
         return NULL;                                                            \
    }                                                                            \
    virtual Object* apply(Objects* arguments, Environment* env, Object* parent, Object* application); \
};                                                                               \
void initialize##ClassName()                                                     \
{                                                                                \
    procedures.push_back(                                                        \
        pair<Variable*, Object*>(new Variable(name), new ClassName()));          \
}                                                                                \
static struct Wrapper##ClassName                                                 \
{                                                                                \
    Wrapper##ClassName(){initialize##ClassName();}                               \
} initializer_##ClassName;                                                       \
    Object* ClassName::apply(Objects* arguments, Environment* env, Object* parent, Object* application)

#endif // __PRIMITIVE_PROCEDURE_PROCEDURE_H__
