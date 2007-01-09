#ifndef __PRIMITIVE_PROCEDURE_NUMBER_EQUAL_H__
#define __PRIMITIVE_PROCEDURE_NUMBER_EQUAL_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class NumberEqual : public PrimitiveProcedure
{
public:
    NumberEqual();
    virtual ~NumberEqual();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_NUMBER_EQUAL_H__
