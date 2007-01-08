#ifndef __SET_H__
#define __SET_H__

#include "PrimitiveProcedure.h"
#include "Number.h"
#include "Variable.h"
#include "Environment.h"

namespace monash {

class Set : public PrimitiveProcedure
{
public:
    Set();
    virtual ~Set();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __SET_H__
