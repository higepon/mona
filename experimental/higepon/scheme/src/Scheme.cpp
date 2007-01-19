#include "Scheme.h"

using namespace monash;

Scheme::Scheme()
{
}

Scheme::~Scheme()
{
}

Object* Scheme::evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* Scheme::listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

#include <map>
// GCの特性を考えて単方向リストにする
// env を保持する必要あり
Objects continuations;
typedef std::map<Object*, Objects*> ProcedureMap;
ProcedureMap proceduresMap;
Object* Scheme::apply(Object* procedure, Objects* arguments, Environment* env)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    static int i = 0;
    static Object* pr = NULL;
    i++;
    if (i == 1) pr = procedure;

    // procesures map
    proceduresMap.insert(std::pair<Object*, Objects*>(procedure, arguments));
    // continuations
    continuations.push_back(procedure);
    for (Objects::const_iterator i = arguments->begin(); i != arguments->end(); ++i)
    {
        continuations.push_back(*i);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if ((*i)->isNumber())
        {
            Number* n = (Number*)(*i);
            printf("%s %s:%d<<%d>>\n", __func__, __FILE__, __LINE__, n->value());fflush(stdout);// debug
            if (n->value() == 3)
            {
                Objects* a = proceduresMap[pr];
                for(Objects::iterator j = a->begin(); j != a->end(); ++j)
                {
                    printf("arg = %s\n", (*j)->toString().c_str());fflush(stdout);
                }
                printf("%s %s:%d[%s]\n", __func__, __FILE__, __LINE__, apply(pr, proceduresMap[pr], env)->toString().c_str());fflush(stdout);// debug
                exit(-1);
                return apply(pr, proceduresMap[pr], env);
            }
        }
    }


    printf("*** Cont ****\n");
    for (Objects::const_iterator i = continuations.begin(); i != continuations.end(); ++i)
    {
        printf("%s\n", (*i)->toString().c_str());
    }

    if (procedure->isCompoundProcedure())
    {
        Objects* as = Scheme::listOfValues(arguments, env);
        Procedure* p = (Procedure*)procedure;
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
        return Scheme::evalSequence(p->body(), e);
    }
    else if (procedure->isPrimitiveProcedure())
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
