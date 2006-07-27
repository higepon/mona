#ifndef __STRING_P_H__
#define __STRING_P_H__

#include "PrimitiveProcedure.h"
#include "String.h"
#include "Number.h"

namespace monash {

class StringP : public PrimitiveProcedure
{
public:
    StringP();
    virtual ~StringP();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments);
};

}; // namespace monash

#endif // __STRING_P_H__
