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
#include<Sub.h>

void* operator new(unsigned long size) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    return (void*)mm.allocateMemory(size);
}

void operator delete(void* address) {

    X86MemoryManager& mm = X86MemoryManager::instance();
    mm.freeMemory((unsigned long)address);
    return;
}

int getNumber() {
    Sub& sub = Sub::instance();
    return sub.getNumber();
}

char* getName() {
    Sub& sub = Sub::instance();
    return sub.getName();
}
