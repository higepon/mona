#include "Kernel.h"

using namespace monash;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

Object* Kernel::evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* Kernel::listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

#include <map>
#include <list>
// GCの特性を考えて単方向リストにする
// env を保持する必要あり

// class Arugment
// {
// public:
//     Arugment(Object* objects) : object(object) {}
//     virtual ~Arugment() {}

//     Object* object;
//     Arugment* next;
// };

// class Continuation
// {
// public:
//     Continuation(Object* object, Environment* env, uint32_t length, bool evalMe = true) : object(object), env(env), length(length), evalMe(evalMe) {}
//     virtual ~Continuation() {}

//     bool evalMe;
//     Object* object;
//     Environment* env;
//     Continuation* next;
//     uint32_t length;
// };


// Arugment* asp = NULL;
// Arugment* abottom = NULL;


// Continuation* sp = NULL;
// Continuation* bottom = NULL;

// void pushArgument(Arugment* argument)
// {
//     if (NULL == asp)
//     {

//         asp = argument;
//     }
//     if (NULL == abottom)
//     {
//         abottom = argument;
//     }
//     else
//     {
//         abottom->next = argument;
//         abottom = argument;
//     }
// }

// Arugment* popArgument()
// {
//     if (NULL == asp) return NULL;
//     Arugment* ret = asp;
//     asp = ret->next;
//     return ret;
// }


// void pushContinuation(Continuation* continuation)
// {
//     if (NULL == sp)
//     {

//         sp = continuation;
//     }
//     if (NULL == bottom)
//     {
//         bottom = continuation;
//     }
//     else
//     {
//         bottom->next = continuation;
//         bottom = continuation;
//     }
// }

// Continuation* popContinuation()
// {
//     if (NULL == sp) return NULL;
//     Continuation* ret = sp;
//     sp = ret->next;
//     return ret;
// }

Object* Kernel::apply(Object* procedure, Objects* arguments, Environment* env, Object* parent, Object* application)
{
//     static int i = 0;
//     static Object* pr = NULL;
//     i++;
//     if (i == 1) pr = procedure;

//     for (Objects::const_iterator p = arguments->begin(); p != arguments->end(); ++p)
//     {
//         // もしも特殊形式だったらひきすうすタックに積むときに eval しない
//         if (0) {
//             pushContinuation(new Continuation(*p, env, 0, false));
//         }
//         else
//         {
//             pushContinuation(new Continuation(*p, env, 0));
//         }
//     }

//     pushContinuation(new Continuation(procedure, env, arguments->size()));

#if 1
    if (procedure->isCompoundProcedure())
    {
        Objects* as = Kernel::listOfValues(arguments, env);
        Procedure* p = (Procedure*)procedure;
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
        return Kernel::evalSequence(p->body(), e);
    }
    else if (procedure->isPrimitiveProcedure())
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env, parent, application);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().c_str());
        return NULL;
    }
#endif
}

// Object* Kernel::doContinuation()
// {
//     for (Continuation* c = popContinuation(); c != NULL; c = popContinuation())
//     {
//         Object* object = c->object;
//         if (object->isCompoundProcedure())
//         {
//             Objects* as = new Objects;
//             for (uint32_t i = c->length - 1 ; i >= 0; i++)
//             {
//                 as->push_back(popArgument()->object);
//             }
//             Procedure* p = (Procedure*)object;
//             Environment* e = p->env()->clone();
//             e->extend(p->parameters(), as); // doubt? we need copy?
//             pushArgument(new Arugment(Kernel::evalSequence(p->body(), e)));
//         }
//         else if (object->isPrimitiveProcedure())
//         {
//             Objects* as = new Objects;
//             for (uint32_t i = c->length - 1 ; i >= 0; i++)
//             {
//                 as->push_back(popArgument()->object);
//             }
//             PrimitiveProcedure* p = (PrimitiveProcedure*)object;
//             return p->apply(as, c->env);
//         }
//         else
//         {
//             if (c->evalMe)
//             {
//                 pushArgument(new Arugment(object->eval(c->env)));
//             }
//             else
//             {
//                 pushArgument(new Arugment(object));
//             }
//         }
//     }
// }
