#ifndef _KERNEL_
#define _KERNEL_

#include "Object.h"
#include "Procedure.h"

namespace monash {

class Kernel
{
public:
    Kernel();
    virtual ~Kernel();

    static Object* evalSequence(Objects* exps, Environment* env);
    static Objects* listOfValues(Objects* objects, Environment* env);
    static Object* apply(Object* procedure, Objects* arguments, Environment* env, Object* parent, Object* application);
    static Object* doContinuation();
};

}; // namespace monash

#endif // _KERNEL_
