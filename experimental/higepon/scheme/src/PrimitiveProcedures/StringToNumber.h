#ifndef __STRING_TO_NUMBER__
#define __STRING_TO_NUMBER__

#include "PrimitiveProcedure.h"
#include "String.h"
#include "Number.h"

namespace monash {

class StringToNumber : public PrimitiveProcedure
{
public:
    StringToNumber();
    virtual ~StringToNumber();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments);
};

}; // namespace monash

#endif // __STRING_TO_NUMBER__
