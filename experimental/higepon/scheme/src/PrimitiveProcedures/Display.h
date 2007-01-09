#ifndef __PRIMITIVE_PROCEDURE_DISPLAY_H__
#define __PRIMITIVE_PROCEDURE_DISPLAY_H__

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

#endif // __PRIMITIVE_PROCEDURE_DISPLAY_H__
