/*!
    \file   purevirtual.cpp
    \brief  purevirtual function

    using pure virtual function, we need this function
    thank you OS chyou teacher@os board

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/

#include <kernel.h>
#include <global.h>

extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);
extern "C" int atexit( void (*func)(void));
extern "C" int __cxa_atexit();
extern "C" void* __dso_handle;
/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/08/04 update:2002/02/25
*/
void __cxa_pure_virtual() {

    g_console->printf("__cxa_pure_virtual called\n");
}

/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/09/07 update: 2003/02/25
*/
void _pure_virtual() {

    g_console->printf("_pure_virtual called\n");
}

void __pure_virtual() {

    g_console->printf("_pure_virtual called\n");
}

/*!
    \brief dummy

    dummy

    \author HigePon
    \date   create:2002/08/08 update:2002/02/25
*/
int atexit( void (*func)(void)) {return -1;}

/*!
    \brief dummy for gcc3.3

    dummy

    \author HigePon
    \date   create:2003/10/13 update:
*/
int __cxa_atexit() {return -1;}

#ifndef GCC29
void*   __dso_handle = (void*) &__dso_handle;
#endif

void monamain() {

    asm volatile("nop");
    /* dummy */
}
