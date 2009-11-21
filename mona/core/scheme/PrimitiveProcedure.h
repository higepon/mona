/*!
    \file  PrimitiveProcedure.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __PRIMITIVEPROCEDURE_H__
#define __PRIMITIVEPROCEDURE_H__

#include "Object.h"
#include <stdio.h>

monash::Objects* listOfValues(monash::Objects* objects, monash::Environment* env);

namespace monash {

class PrimitiveProcedure : public Object
{
public:
    PrimitiveProcedure() {}
    virtual ~PrimitiveProcedure() {}

    virtual ::util::String toString() = 0;
    virtual int type() const { return Object::PRIMITIVE_PROCEDURE; }
    virtual uint32_t lineno() const { return 0; }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true) = 0;
    virtual Object* eval(Environment* env)                      = 0;
};

}; // namespace monash

#endif // __PRIMITIVEPROCEDURE_H__
