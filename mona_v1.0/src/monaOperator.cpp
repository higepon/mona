/*!
    \file   monaOperator.cpp
    \brief  define operator new & delete

    define operator new & delete

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/08 update:$Date$
*/
#include<IA32MemoryManager.h>
#include<monaOperator.h>
#include<monaTypes.h>
#include<VirtualConsole.h>

extern VirtualConsole* console;

#ifndef BUILD_ON_LINUX
void* operator new(size_t size) {

    console->printf("new size = %d\n", size);

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.printInfo("new");
    return mm.allocateMemory(size);
}

void operator delete(void* address) {

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.freeMemory(address);
    return;
}

void* operator new[](size_t size) {

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    return mm.allocateMemory(size);
}

void operator delete[](void* address) {

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.freeMemory(address);
    return;
}

#endif

void* malloc(unsigned long size) {

    console->printf("malloc size = %d\n", size);
    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.printInfo("malloc");
    return mm.allocateMemory(size);
}

void __builtin_delete(void* address) {

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.freeMemory(address);
    return;
}

void* __builtin_new(unsigned long size) {

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    return mm.allocateMemory(size);
}

void* __builtin_vec_new(unsigned long size) {

    return __builtin_new(size);
}

void __builtin_vec_delete(void* address) {

    __builtin_delete(address);
    return;
}
