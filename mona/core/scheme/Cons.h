/*!
    \file  Cons.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __CONS_H__
#define __CONS_H__

#include "Object.h"
#include "Number.h"
#include "Variable.h"
#include "Identifier.h"

namespace monash {

class Cons : public Object
{
public:
    Cons(Object* car, Object* cdr, uint32_t lineno = 0);
    Cons(uint32_t lineno = 0);
    virtual ~Cons();

    Object* getCar() const {return car_;}
    Object* getCdr() const {return cdr_;}
    Object* getCddr() const {return getNthCdr(1);}
    Object* getCdddr() const {return getNthCdr(2);}
    Object* getCadr() const {return getNthCar(1);}
    Object* getCaddr() const {return getNthCar(2);}
    Object* getCadddr() const {return getNthCar(3);}
    Object* listRef(uint32_t n) const { return getNthCar(n); }
    void append(Object* o);
    Object* getNthCar(uint32_t n) const;
    Object* getNthCdr(uint32_t n) const;
    void toObjects(Objects* objects);
    void setCar(Object* car) {car_ = car;}
    void setCdr(Object* cdr) {cdr_ = cdr;}

public:
    virtual int getListLength() const;
    virtual ::util::String toString();
    virtual ::util::String toStringInternal(bool inList = false);
    virtual ::util::String toStringValue();
    virtual int type() const { return Object::CONS; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "pair"; }
    virtual Object* eval(Environment* env);
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    Object* car_;
    Object* cdr_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __CONS_H__
