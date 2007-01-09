#ifndef __PRIMITIVE_PROCEDURE_CONS_H__
#define __PRIMITIVE_PROCEDURE_CONS_H__

#include "PrimitiveProcedure.h"
#include "Pair.h"

namespace monash {

class Cons : public PrimitiveProcedure
{
public:
    Cons();
    virtual ~Cons();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_CONS_H__
