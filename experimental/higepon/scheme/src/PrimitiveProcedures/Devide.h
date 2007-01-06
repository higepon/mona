#ifndef __DEVIDE_H__
#define __DEVIDE_H__

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

#endif // __DEVIDE_H__
