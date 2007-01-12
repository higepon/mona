#ifndef _PRIMITIVE_PROCEDURE_CHARCTERP_
#define _PRIMITIVE_PROCEDURE_CHARCTERP_

#include "Procedure.h"
#include "Charcter.h"
#include "True.h"
#include "False.h"
#include "Vector.h"

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
PROCEDURE_DECLARE(CharcterToInteger)
PROCEDURE_DECLARE(IntegerToCharcter)
PROCEDURE_DECLARE(VectorP)
PROCEDURE_DECLARE(MakeVector)
PROCEDURE_DECLARE(CreateVector)
PROCEDURE_DECLARE(VectorLength)
PROCEDURE_DECLARE(VectorRef)
PROCEDURE_DECLARE(VectorSet)


#endif // _PRIMITIVE_PROCEDURE_CHARCTERP_
