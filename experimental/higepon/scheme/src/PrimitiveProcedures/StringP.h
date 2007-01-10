#ifndef __PRIMITIVE_PROCEDURE_STRING_P_H__
#define __PRIMITIVE_PROCEDURE_STRING_P_H__

#include "PrimitiveProcedure.h"
#include "String.h"
#include "False.h"
#include "True.h"

namespace monash {

class StringP : public PrimitiveProcedure
{
public:
    StringP();
    virtual ~StringP();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_STRING_P_H__
