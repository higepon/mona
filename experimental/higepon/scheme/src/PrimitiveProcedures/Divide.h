#ifndef __PRIMITIVE_PROCEDURE_DIVIDE_H__
#define __PRIMITIVE_PROCEDURE_DIVIDE_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Divide : public PrimitiveProcedure
{
public:
    Divide();
    virtual ~Divide();

    virtual std::string toString();
//    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PRIMITIVE_PROCEDURE_DIVIDE_H__
