/*!
    \file  True.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _PRIMITIVE_PROCEDURE_TRUE_
#define _PRIMITIVE_PROCEDURE_TRUE_

#include "PrimitiveProcedure.h"

namespace monash {

class True : public PrimitiveProcedure
{
public:
    True();
    virtual ~True();

    virtual ::util::String toString();
    virtual ::util::String typeString() const { return "#t"; }
    virtual int type() const { return Object::TRUE; }
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
    virtual bool eqv(Object* o);
};

}; // namespace monash

#endif // _PRIMITIVE_PROCEDURE_TRUE_
