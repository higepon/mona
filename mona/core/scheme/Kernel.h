/*!
    \file  Kernel.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _KERNEL_
#define _KERNEL_

#include "Object.h"
#include "Procedure.h"
//#include <algorithm>

namespace monash {

typedef struct Argument
{
    Object* object;
    Argument* prev;
} Argument;

class Kernel
{
public:
    Kernel();
    virtual ~Kernel();

    static Object* evalSequence(Objects* exps, Environment* env);
    static Objects* listOfValues(Objects* objects, Environment* env);
    static Object* apply(Object* procedure, Objects* arguments, Environment* env);
    static Object* doContinuation();
    static void makeListOfValues(Objects* objects, int i, Argument* prev, Environment* environment, Objects** values);
};

}; // namespace monash

#endif // _KERNEL_
