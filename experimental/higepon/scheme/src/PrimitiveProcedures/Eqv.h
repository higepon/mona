#ifndef _PRIMITIVE_PROCEDURE_EQV_
#define _PRIMITIVE_PROCEDURE_EQV_

#include "PrimitiveProcedure.h"
#include "True.h"
#include "False.h"

namespace monash {

class Eqv : public PrimitiveProcedure
{
public:
    Eqv();
    virtual ~Eqv();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // _PRIMITIVE_PROCEDURE_EQV_
