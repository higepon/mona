#ifndef __CONS_H__
#define __CONS_H__

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
    virtual Object* apply(Objects* arguments);
};

}; // namespace monash

#endif // __CONS_H__
