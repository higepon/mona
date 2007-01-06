#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "PrimitiveProcedure.h"
#include "Number.h"

namespace monash {

class Display : public PrimitiveProcedure
{
public:
    Display();
    virtual ~Display();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash

#endif // __DISPLAY_H__
