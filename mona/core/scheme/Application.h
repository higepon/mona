/*!
    \file  Application.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Object.h"
#include "Environment.h"
#include "Cons.h"

namespace monash {

class Application : public Object
{
public:
    Application(Object* operatorr, Cons* operands, uint32_t lineno = 0);
    virtual ~Application();

    virtual ::util::String toString();
    virtual int type() const { return Object::APPLICATION; }
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "application"; }
    Object* operatorr() { return operatorr_; }
    Cons* operands() { return operands_; }

protected:
    Object* operatorr_;
    Cons* operands_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __APPLICATION_H__
