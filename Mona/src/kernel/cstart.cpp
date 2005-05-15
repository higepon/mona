/*!
    \file   cstart.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

extern void startKernel();

/*!
    \brief  mona kernel start at this point

    mona kernel start at this point.

    \author HigePon
    \date   create:2002/07/21 update:
*/
extern "C" void cstart()
{
    startKernel();
}
