#ifndef __PRIMITIVE_PROCEDURE_CDR_H__
#define __PRIMITIVE_PROCEDURE_CDR_H__

#include "PrimitiveProcedure.h"
#include "Pair.h"

namespace monash {

class Cdr : public PrimitiveProcedure
{
public:
    Cdr();
    virtual ~Cdr();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_CDR_H__
