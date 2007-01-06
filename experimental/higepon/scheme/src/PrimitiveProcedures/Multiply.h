#ifndef __MULTIPLY_H__
#define __MULTIPLY_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Multiply : public PrimitiveProcedure
{
public:
    Multiply();
    virtual ~Multiply();

    virtual std::string toString();
//    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __MULTIPLY_H__
