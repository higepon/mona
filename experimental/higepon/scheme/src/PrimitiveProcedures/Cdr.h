#ifndef __CDR_H__
#define __CDR_H__

#include "PrimitiveProcedure.h"
#include "Pair.h"

namespace monash {

class Cdr : public PrimitiveProcedure
{
public:
    Cdr();
    virtual ~Cdr();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments);
};

}; // namespace monash

#endif // __CDR_H__
