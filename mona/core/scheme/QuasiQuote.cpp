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

Object* QuasiQuote::evalIter(Object* object, Environment* env)
{
    if (object->isUnquote())
    {
        return object->eval(env);
    }
    else if (object->isPair())
    {
        Pair* pair = (Pair*)object;
        Pair* prevPair = NULL;
        for (;;)
        {
            Object* car = pair->getCar();
            if (car->isUnquote())
            {
                pair->setCar(car->eval(env));
            }
            else if (car->isUnquoteSplicing())
            {
                Object* evaluated = car->eval(env);
                if (evaluated->isPair())
                {
                    Pair* evaluatedPair = (Pair*)evaluated;
                    Object* cdr = pair->getCdr();
                    pair->setCar(evaluatedPair->getCar());
                    pair->setCdr(evaluatedPair->getCdr());

                    Pair* start = pair;
                    for (;;)
                    {
                        if (start->getCdr()->isNil())
                        {
                            start->setCdr(cdr);
                            break;
                        }
                        Object* tmp = start->getCdr();
                        if (!tmp->isPair())
                        {
                            RAISE_ERROR(object->lineno(), "unquote-splicing got not pair!");
                        }
                        start = (Pair*)tmp;
                    }
                }
                else if (evaluated->isNil())
                {
//                    Pair* p = (Pair*)prevPair->getCdr();
                    prevPair->setCdr(pair->getCdr());
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
            if (!cdr->isPair())
            {
                pair->setCdr(evalIter(pair->getCdr(), env));
                break;
            }
            prevPair = pair;
            pair = (Pair*)cdr;
        }
        return object;
    }
    else
    {
        return object;
    }
}

Object* QuasiQuote::eval(Environment* env)
{
    return evalIter(object_, env);
}

bool QuasiQuote::eqv() const
{
    return false;
}

bool QuasiQuote::eq() const
{
    return false;
}
