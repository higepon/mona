/*!
    \file   monaPureVirtual.cpp
    \brief  purevirtual function

    using pure virtual function, we need this function
    thank you OS chyou teacher@os board

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/

#include<monaVga.h>

extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" int atexit( void (*func)(void));
/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/08/04 update:
*/
void __cxa_pure_virtual() {

    _sysPrintln("__cxa_pure_virtual called");
}

/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/09/07 update:
*/
void _pure_virtual() {

    _sysPrintln("_pure_virtual called");
}

/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/08/08 update:2002/12/15
*/
int atexit( void (*func)(void)) {}
