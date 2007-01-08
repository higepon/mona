#ifndef __DIVIDE_H__
#define __DIVIDE_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Devide : public PrimitiveProcedure
{
public:
    Devide();
    virtual ~Devide();

    virtual std::string toString();
//    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __DIVIDE_H__
