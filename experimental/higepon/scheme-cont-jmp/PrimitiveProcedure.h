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

    virtual std::string toString() = 0;
    virtual int type() const { return Object::PRIMITIVE_PROCEDURE; }
    virtual uint32_t lineno() const { return 0; }
    virtual Object* eval(Environment* env)                      = 0;
};

}; // namespace monash

#endif // __PRIMITIVEPROCEDURE_H__
