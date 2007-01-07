#ifndef __STRING_APPEND__
#define __STRING_APPEND__

#include "PrimitiveProcedure.h"
#include "String.h"

namespace monash {

class StringAppend : public PrimitiveProcedure
{
public:
    StringAppend();
    virtual ~StringAppend();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __STRING_APPEND__
