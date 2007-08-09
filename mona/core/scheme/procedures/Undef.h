/*!
    \file  Undef.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _PRIMITIVE_PROCEDURE_UNDEF_
#define _PRIMITIVE_PROCEDURE_UNDEF_

#include "PrimitiveProcedure.h"

namespace monash {

class Undef : public PrimitiveProcedure
{
public:
    Undef();
    virtual ~Undef();

    virtual ::util::String toString();
    virtual ::util::String typeString() const { return "undef"; }
    virtual int type() const { return Object::UNDEF; }
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
    virtual bool eqv(Object* o);
};

}; // namespace monash

#endif // _PRIMITIVE_PROCEDURE_UNDEF_
