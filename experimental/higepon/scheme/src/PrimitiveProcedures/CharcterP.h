#ifndef _PRIMITIVE_PROCEDURE_CHARCTERP_
#define _PRIMITIVE_PROCEDURE_CHARCTERP_

#include "Procedure.h"
#include "Charcter.h"
#include "True.h"
#include "False.h"
#include "Vector.h"
#include "String.h"
#include "Quote.h"

namespace monash {

class CharcterP : public PrimitiveProcedure
{
public:
    CharcterP();
    virtual ~CharcterP();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

}; // namespace monash


#endif // _PRIMITIVE_PROCEDURE_CHARCTERP_
