/*!
    \file   higeOperator.cpp
    \brief  define operator new & delete

    define operator new & delete

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/08 update:$Date$
*/
#include<X86MemoryManager.h>
#include<higeOperator.h>

#ifndef BUILD_ON_LINUX
void* operator new(unsigned int size) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    return mm.allocateMemory(size);
}

void operator delete(void* address) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    mm.freeMemory(address);
    return;
}
#endif

void* malloc(unsigned long size) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    return mm.allocateMemory(size);
}

void __builtin_delete(void* address) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    mm.freeMemory(address);
    return;
}

void* __builtin_new(unsigned long size) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    return mm.allocateMemory(size);
}

void* __builtin_vec_new(unsigned long size) {

    return __builtin_new(size);
}
