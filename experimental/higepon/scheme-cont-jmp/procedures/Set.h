#ifndef __PRIMITIVE_PROCEDURE_SET_H__
#define __PRIMITIVE_PROCEDURE_SET_H__

#include "PrimitiveProcedure.h"
#include "Number.h"
#include "Variable.h"
#include "Environment.h"

namespace monash {

class Set : public PrimitiveProcedure
{
public:
    Set();
    virtual ~Set();

    virtual ::util::String toString();
    virtual ::util::String typeString() const { return "set! proc"; }
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_SET_H__
