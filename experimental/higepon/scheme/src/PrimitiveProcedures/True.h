#ifndef _PRIMITIVE_PROCEDURE_TRUE_
#define _PRIMITIVE_PROCEDURE_TRUE_

#include "PrimitiveProcedure.h"

namespace monash {

class True : public PrimitiveProcedure
{
public:
    True();
    virtual ~True();

    virtual std::string toString();
    virtual int type() const { return Object::TRUE; }
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
    virtual bool eqv(Object* o);
};

}; // namespace monash

#endif // _PRIMITIVE_PROCEDURE_TRUE_
