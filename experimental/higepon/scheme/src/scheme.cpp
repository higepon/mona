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

Object* apply(Object* procedure, Objects* arguments)
{
    if (isCompoundProcedure(procedure))
    {
        Procedure* p = (Procedure*)procedure;
//        p->env()->extend(p->parameters(), arguments); // doubt? we need copy?
        Environment* env = p->env()->clone();
        env->extend(p->parameters(), arguments); // doubt? we need copy?
// bug?
        //evalSequence(p->body(), env);
        return evalSequence(p->body(), env);
    }
    else if (isPrimitiveProcedure(procedure))
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments);
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
