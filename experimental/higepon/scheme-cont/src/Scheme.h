#ifndef _SCHEME_
#define _SCHEME_

#include "Object.h"
#include "Procedure.h"

namespace monash {

class Scheme
{
public:
    Scheme();
    virtual ~Scheme();

    static Object* evalSequence(Objects* exps, Environment* env);
    static Objects* listOfValues(Objects* objects, Environment* env);
    static Object* apply(Object* procedure, Objects* arguments, Environment* env);
    static Object* doContinuation();
};

}; // namespace monash

#endif // _SCHEME_
