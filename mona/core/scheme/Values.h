/*!
    \file  Values.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __VALUES_H__
#define __VALUES_H__

#include "Object.h"
#include "Charcter.h"

namespace monash {

class Values : public Object
{
public:
    Values(Objects* values, uint32_t lineno = 0);
    virtual ~Values();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual ::util::String typeString() const { return "values"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* eval(Environment* env);
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual Objects* values() { return values_; }

protected:
    Objects* values_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __OUTPUT_PORT_H__
