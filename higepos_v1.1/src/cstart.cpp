/*!
    \file   cstart.cpp
    \brief  higepos kernel start at this point

    higepos kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/
#include <higeKernel.h>

/*!
    \brief  higepos kernel start at this point

    higepos kernel start at this point.

    \author HigePon
    \date   create:2002/07/21 update:
*/
void cstart(void) {
    startKernel();
}
