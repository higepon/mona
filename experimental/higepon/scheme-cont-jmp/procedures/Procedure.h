#ifndef __PRIMITIVE_PROCEDURE_PROCEDURE_H__
#define __PRIMITIVE_PROCEDURE_PROCEDURE_H__

#include "PrimitiveProcedure.h"
#include "Kernel.h"
#include "scheme.h"


#define ARGV(i) (as->get(i))
#define ARGC    (as->size())

inline void scheme_gc_init()
{
#ifdef USE_BOEHM_GC
    GC_INIT();
#else
#endif
}

#define RETURN_BOOLEAN(condition) \
    if ((condition))              \
    {                             \
        return SCM_TRUE;          \
    }                             \
    else                          \
    {                             \
        return SCM_FALSE;         \
    }

#define CAST(o, type, to)                                                        \
    if (!o->is##type())                                                          \
    {                                                                            \
        RAISE_ERROR(o->lineno(), "%s got wrong argument. got %s but required " #type ""  \
                    , toString().data(), o->typeString().data());                \
                                                                                 \
    }                                                                            \
    type* to = (type*)o;

#define CAST_RETURN_FALSE(o, type, to)                                           \
    if (!o->is##type())                                                          \
    {                                                                            \
        return SCM_FALSE;                                                        \
    }                                                                            \
    type* to = (type*)o;

#define ARGC_SHOULD_BE(n)                                                        \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC != n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0                            \
                    , "%s got %d argument(s), but required %d"                   \
                    , toString().data(), ARGC, n);                               \
    }

#define ARGC_SHOULD_BE_GT(n)                                                     \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                             \
                    , "%s got %d argument(s), but required at least %d"          \
                    , toString().data(), ARGC, n + 1);                           \
    }

#define ARGC_SHOULD_BE_GT_RETURN_FALSE(n)                                        \
    Objects* as = Kernel::listOfValues(arguments, env);                          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        return SCM_FALSE;                                                        \
    }


#define ARGC_SHOULD_BE_BETWEEN(m, n)                                                         \
    Objects* as = Kernel::listOfValues(arguments, env);                                      \
    if (ARGC < m || ARGC > n)                                                                \
    {                                                                                        \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                                         \
                    , "%s got %d argument(s), but required between %d and %d arguments."     \
                    , toString().data(), ARGC, m, n);                                        \
    }


#define PROCEDURE(ClassName, name)                                               \
class ClassName : public PrimitiveProcedure                                      \
{                                                                                \
private:                                                                         \
    ::util::String procedureName_;                                               \
public:                                                                          \
    ClassName()  : procedureName_(name) {}                                       \
    virtual ~ClassName() {}                                                      \
                                                                                 \
    virtual ::util::String toString() {    return "procedure:" name;}            \
    virtual ::util::String typeString() const { return name; }                   \
    virtual Object* eval(Environment* env)                                       \
    {                                                                            \
        return this;                                                             \
    }                                                                            \
    virtual Object* apply(Objects* arguments, Environment* env);                 \
};                                                                               \
void initialize##ClassName()                                                     \
{                                                                                \
    if (!g_gc_initialized)                                                       \
    {                                                                            \
        scheme_gc_init();                                                        \
        g_gc_initialized = true;                                                 \
    }                                                                            \
    procedures.add(                                                              \
        ::util::Pair<Variable*, Object*>(new Variable(name), new ClassName()));  \
}                                                                                \
static struct Wrapper##ClassName                                                 \
{                                                                                \
    Wrapper##ClassName(){initialize##ClassName();}                               \
} initializer_##ClassName;                                                       \
    Object* ClassName::apply(Objects* arguments, Environment* env)

#endif // __PRIMITIVE_PROCEDURE_PROCEDURE_H__
