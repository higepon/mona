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
//        p->env()->extend(p->parameters(), arguments); // doubt? we need copy?
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
//        printf("###############################################################################\n");fflush(stdout);
        Variables* vs = p->parameters();
//         for (Variables::const_iterator i = vs->begin(); i != vs->end(); ++i)
//         {
//             printf("variable %s\n", (*i)->toString().c_str());fflush(stdout);
//         }

//         for (Objects::const_iterator i = as->begin(); i != as->end(); ++i)
//         {
//             printf("object %x\n",*i);fflush(stdout);
//         }
        

// bug?
        //evalSequence(p->body(), env);
        return evalSequence(p->body(), e);
    }
    else if (isPrimitiveProcedure(procedure))
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env);
    }
    else
    {
        printf("unknown procedure\n");
        return NULL;
    }
}

bool isTrue(Object* exp)
{
    if (exp->type() == Object::NUMBER)
    {
        Number* n = (Number*)exp;
        if (n->value() == 0) return false;
    }
    return true;
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
    env->defineVariable(new Variable("/"),              new Devide());
    env->defineVariable(new Variable("="),              new NumberEqual());
    env->defineVariable(new Variable("cons"),           new Cons());
    env->defineVariable(new Variable("car"),            new Car());
    env->defineVariable(new Variable("cdr"),            new Cdr());
    env->defineVariable(new Variable("display"),        new Display());
    env->defineVariable(new Variable("string?"),        new StringP());
    env->defineVariable(new Variable("number?"),        new NumberP());
    env->defineVariable(new Variable("string->number"), new StringToNumber());
    env->defineVariable(new Variable("string-append"),  new StringAppend());
    env->defineVariable(new Variable("#f"),             new Number(0));
    env->defineVariable(new Variable("#t"),             new Number(1));
    env->defineVariable(new Variable("set!"),           new Set());
}
