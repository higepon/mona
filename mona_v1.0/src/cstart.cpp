/*!
    \file   cstart.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/
#include <kernel.h>

/*!
    \brief  mona kernel start at this point

    mona kernel start at this point.

    \author HigePon
    \date   create:2002/07/21 update:
*/
void cstart(void) {
    startKernel();
}
