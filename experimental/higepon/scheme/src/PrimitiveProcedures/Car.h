#ifndef __CAR_H__
#define __CAR_H__

#include "PrimitiveProcedure.h"
#include "Pair.h"

namespace monash {

class Car : public PrimitiveProcedure
{
public:
    Car();
    virtual ~Car();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __CAR_H__
