/*!
    \file   operator.cpp
    \brief  define operator new & delete

    define operator new & delete

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/08 update:$Date$
*/
#include<MemoryManager.h>
#include<operator.h>
#include<types.h>
#include<global.h>

#ifndef BUILD_ON_LINUX
void* operator new(size_t size) {
    return km.allocate(size);
}

void operator delete(void* address) {
    km.free(address);
    return;
}

void* operator new[](size_t size) {
    return km.allocate(size);
}

void operator delete[](void* address) {

    km.free(address);
    return;
}

#else

void __builtin_delete(void* address) {

    km.free(address);
    return;
}

void* __builtin_new(unsigned long size) {

    return km.allocate(size);
}

void* __builtin_vec_new(unsigned long size) {

    return __builtin_new(size);
}

void __builtin_vec_delete(void* address) {

    __builtin_delete(address);
    return;
}

#endif

void* malloc(unsigned long size) {

    return km.allocate(size);
}

void free(void * address) {

    km.free(address);
    return;
}



