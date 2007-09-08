/*!
    \file   Cons.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Cons.h"

//using namespace util;
using namespace monash;

Cons::Cons(Object* car, Object* cdr, uint32_t lineno) : car_(car), cdr_(cdr), lineno_(lineno)
{
}

Cons::Cons(uint32_t lineno /* = 0 */) : car_(NULL), cdr_(SCM_NIL), lineno_(lineno)
{
}


Cons::~Cons()
{

}

// this instance should be proper list
int Cons::getListLength() const
{
    const Cons* cons = this;
    for (uint32_t length = 1; ; length++)
    {
        Object* cdr = cons->getCdr();
        if (cdr->isNil())
        {
            return length;
        }
        else if (cdr->isCons())
        {
            cons = (Cons*)cdr;
        }
        else
        {
            return -1;
        }
    }
}

// append!
void Cons::append(Object* o)
{
    Cons* cons = this;
    if (cons->getCar() == NULL)
    {
        cons->setCar(o);
        return;
    }
    for (;;)
    {
        Object* cdr = cons->getCdr();
        if (cdr->isNil())
        {
            cons->setCdr(new Cons(o, SCM_NIL));
            return;
        }
        else if (cdr->isCons())
        {
            cons = (Cons*)cdr;
        }
        else
        {
            return;
        }
    }
}

void Cons::toObjects(Objects* objects)
{
    Cons* c = this;
    for (;;)
    {
        Object* car = c->getCar();
        Object* cdr = c->getCdr();
        objects->add(car);
        if (cdr->isNil())
        {
            break;
        }
        else if (cdr->isCons())
        {
            c = (Cons*)cdr;
        }
        else
        {
            return;
        }
    }
}

Object* Cons::getNthCar(uint32_t n) const
{
    const Cons* cons = this;
    for (uint32_t i = 0; i < n; i++)
    {
        Object* cdr = cons->getCdr();
        if (cdr->isCons())
        {
            cons = (Cons*)cdr;
        }
        else
        {
            return NULL;
        }
    }
    return cons->getCar();
}

Object* Cons::getNthCdr(uint32_t n) const
{
    const Cons* cons = this;
    for (uint32_t i = 0; i < n; i++)
    {
        Object* cdr = cons->getCdr();
        if (cdr->isCons())
        {
            cons = (Cons*)cdr;
        }
        else
        {
            return NULL;
        }
    }
    return cons->getCdr();
}

::util::String Cons::toString()
{
    return toStringInternal(false);
}

::util::String Cons::toStringInternal(bool inList /* = false */)
{
    ::util::String carString = car_ ? car_->toString() : "NULL";
    if (cdr_->isCons())
    {
        Cons* p = (Cons*)cdr_;
        ::util::String cdrString = p->toStringInternal(true);
        if (inList)
        {
            return carString + " " + cdrString;
        }
        else
        {
            if (carString == "quote")
            {
                return cdrString;
            }
            else
            {
                return "(" + carString + " " + cdrString + ")";
            }
        }
    }
    else if (cdr_->isNil())
    {
        if (inList)
        {
            return carString;
        }
        else
        {
            return "(" + carString + ")";
        }
    }
    else
    {
        ::util::String cdrString = cdr_->toString();
        if (inList)
        {
            return carString + " . " + cdrString;
        }
        else
        {
            return "(" + carString + " . " + cdrString + ")";
        }
    }
}

::util::String Cons::toStringValue()
{
    return toString();
}

Object* Cons::eval(Environment* env)
{
    return this;
}

bool Cons::eqv(Object* o)
{
    if (!o->isCons()) return false;
    Cons* p = (Cons*)o;
    return this == p;
}

bool Cons::eq(Object* o)
{
    return eqv(o);
}
