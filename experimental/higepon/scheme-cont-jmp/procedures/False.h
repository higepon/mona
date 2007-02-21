#ifndef _PRIMITIVE_PROCEDURE_FALSE_
#define _PRIMITIVE_PROCEDURE_FALSE_

#include "PrimitiveProcedure.h"

namespace monash {

class False : public PrimitiveProcedure
{
public:
    False(uint32_t lineno = 0);
    virtual ~False();

    virtual ::util::String toString();
    virtual int type() const { return Object::FALSE; }
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "#f"; }
    virtual Object* apply(Objects* arguments, Environment* env);
    bool eqv(Object* o);

protected:
    uint32_t lineno_;
};

}; // namespace monash

#endif // _PRIMITIVE_PROCEDURE_FALSE_
