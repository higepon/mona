#ifndef __PRIMITIVEPROCEDURE_H__
#define __PRIMITIVEPROCEDURE_H__

#include "Object.h"

namespace monash {

class PrimitiveProcedure : public Object
{
public:
    PrimitiveProcedure() {}
    virtual ~PrimitiveProcedure() {}

    virtual std::string toString() = 0;
    virtual int type() const { return Object::PRIMITIVE_PROCEDURE; }
    virtual Object* eval(Environment* env)    = 0;
    virtual Object* apply(Objects* arguments) = 0;
};

}; // namespace monash

#endif // __PRIMITIVEPROCEDURE_H__