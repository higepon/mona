#ifndef __PLUS_H__
#define __PLUS_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Plus : public PrimitiveProcedure
{
public:
    Plus();
    virtual ~Plus();

    virtual std::string toString();
//    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __PLUS_H__
