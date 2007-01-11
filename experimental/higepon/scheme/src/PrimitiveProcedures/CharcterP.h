#ifndef _PRIMITIVE_PROCEDURE_CHARCTERP_
#define _PRIMITIVE_PROCEDURE_CHARCTERP_

#include "Procedure.h"
#include "Charcter.h"
#include "True.h"
#include "False.h"

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

PROCEDURE_DECLARE(CharcterEqualP)
PROCEDURE_DECLARE(CharcterGtP)
PROCEDURE_DECLARE(CharcterLtP)
PROCEDURE_DECLARE(CharcterGeP)
PROCEDURE_DECLARE(CharcterLeP)


#endif // _PRIMITIVE_PROCEDURE_CHARCTERP_
