/*!
    \file   QuasiQuote.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "QuasiQuote.h"

using namespace util;
using namespace monash;

QuasiQuote::QuasiQuote(Object* object, uint32_t lineno) : object_(object), lineno_(lineno)
{
}

QuasiQuote::~QuasiQuote()
{
}

String QuasiQuote::toString()
{
    return "#<syntax quasiquote>";
}

int QuasiQuote::type() const
{
    return Object::QUASI_QUOTE;
}

// quasiquote will be evaled sometimes.
// we need a copy
Object* QuasiQuote::evalIter(Object* object, Environment* env)
{
    if (object->isUnquote())
    {
        return Kernel::evalTailOpt(object, env);
    }
    else if (object->isCons())
    {
        Cons* pair = (Cons*)object;
        Cons* prevCons = NULL;
        for (;;)
        {
            Object* car = pair->getCar();
            if (car->isUnquote())
            {
                pair->setCar(Kernel::evalTailOpt(car, env));
            }
            else if (car->isUnquoteSplicing())
            {
                Object* evaluated = Kernel::evalTailOpt(car, env);
                if (evaluated->isCons())
                {
                    Cons* evaluatedCons = (Cons*)evaluated;
                    Object* cdr = pair->getCdr();
                    pair->setCar(evaluatedCons->getCar());
                    pair->setCdr(evaluatedCons->getCdr());

                    Cons* start = pair;
                    for (;;)
                    {
                        if (start->getCdr()->isNil())
                        {
                            start->setCdr(cdr);
                            break;
                        }
                        Object* tmp = start->getCdr();
                        if (!tmp->isCons())
                        {
                            RAISE_ERROR(object->lineno(), "unquote-splicing got not pair!");
                        }
                        start = (Cons*)tmp;
                    }
                }
                else if (evaluated->isNil())
                {
//                    Cons* p = (Cons*)prevCons->getCdr();
                    prevCons->setCdr(pair->getCdr());
                }
                else
                {
                    RAISE_ERROR(object->lineno(), "unquote-splicing got not pair!");
                }
            }
            else
            {
                evalIter(car, env);
            }

            Object* cdr = pair->getCdr();
            if (cdr->isNil()) break;
            if (!cdr->isCons())
            {
                pair->setCdr(evalIter(pair->getCdr(), env));
                break;
            }
            prevCons = pair;
            pair = (Cons*)cdr;
        }
        return object;
    }
    else
    {
        return object;
    }
}

// quasiquote will be evaled sometimes.
// we need a copy
Object* QuasiQuote::cloneSexp(Object* o)
{
    if (o->isCons())
    {
        Cons* cons = new Cons(cloneSexp(((Cons*)o)->getCar()), cloneSexp(((Cons*)o)->getCdr()));
        return cons;
    }
    else
    {
        return o;
    }
}

Object* QuasiQuote::eval(Environment* env)
{
    return evalIter(cloneSexp(object_), env);
}

bool QuasiQuote::eqv() const
{
    return false;
}

bool QuasiQuote::eq() const
{
    return false;
}
