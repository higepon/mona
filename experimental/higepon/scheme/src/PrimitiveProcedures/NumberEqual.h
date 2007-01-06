#ifndef __NUMBER_EQUAL_H__
#define __NUMBER_EQUAL_H__

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

#endif // __NUMBER_EQUAL_H__
