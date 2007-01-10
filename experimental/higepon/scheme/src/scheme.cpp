#include "scheme.h"

using namespace std;
using namespace monash;

Object* evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

bool isCompoundProcedure(Object* p)
{
    return p->type() == Object::PROCEDURE;
}

bool isPrimitiveProcedure(Object* p)
{
    return p->type() == Object::PRIMITIVE_PROCEDURE;
}

Object* apply(Object* procedure, Objects* arguments, Environment* env)
{
    if (isCompoundProcedure(procedure))
    {
       Objects* as = listOfValues(arguments, env);
        Procedure* p = (Procedure*)procedure;
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
        return evalSequence(p->body(), e);
    }
    else if (isPrimitiveProcedure(procedure))
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().c_str());
        return NULL;
    }
}

bool isTrue(Object* exp)
{
    return exp->type() != Object::FALSE;
}

void display(Object* exp)
{
    printf("%s", exp->toString().c_str());
}

void registerPrimitives(Environment* env)
{
    env->defineVariable(new Variable("+"),              new Plus());
    env->defineVariable(new Variable("-"),              new Minus());
    env->defineVariable(new Variable("*"),              new Multiply());
    env->defineVariable(new Variable("/"),              new Divide());
    env->defineVariable(new Variable("="),              new NumberEqual());
    env->defineVariable(new Variable("cons"),           new Cons());
    env->defineVariable(new Variable("car"),            new Car());
    env->defineVariable(new Variable("cdr"),            new Cdr());
    env->defineVariable(new Variable("display"),        new Display());
    env->defineVariable(new Variable("string?"),        new StringP());
    env->defineVariable(new Variable("number?"),        new NumberP());
    env->defineVariable(new Variable("string->number"), new StringToNumber());
    env->defineVariable(new Variable("string-append"),  new StringAppend());
    env->defineVariable(new Variable("#f"),             new False());
    env->defineVariable(new Variable("#t"),             new True());
    env->defineVariable(new Variable("set!"),           new Set());
    env->defineVariable(new Variable("eqv?"),           new Eqv());
// load
    env->defineVariable(new Variable("load"),           new Load());
}
