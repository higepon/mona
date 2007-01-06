#ifndef __MINUS_H__
#define __MINUS_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Minus : public PrimitiveProcedure
{
public:
    Minus();
    virtual ~Minus();

    virtual std::string toString();
//    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __MINUS_H__
