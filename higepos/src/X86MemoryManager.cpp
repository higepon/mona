/*!
    \file  MemoryManager.cpp
    \brief class X86MemoryManager

    class X86MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
#include<X86MemoryManager.h>

/*!
    \brief get class Message

    get class Message

    \author HigePon
    \date   create:2002/08/04 update:
*/
char* X86MemoryManager::getMessage() {

    static char buf[] = "x86higepos";
    return buf;
}

/*!
    \brief get class Name

    get class Name

    \author HigePon
    \date   create:2002/08/04 update:
*/
char* X86MemoryManager::getName() {

    static char buf[] = "X86MemoryManager";
    return buf;
}

/*!
    \brief allocate memory

    allocate memory

    \param size size of memory to allocate
    \return address to allocated memory

    \author HigePon
    \date   create:2002/08/07 update:
*/
unsigned long X86MemoryManager::allocateMemory(unsigned long size) {

    return 0;
}

/*!
    \brief free memory

    free memory

    \param address adrress of memory to be free
    \return result

    \author HigePon
    \date   create:2002/08/07 update:
*/
unsigned long X86MemoryManager::freeMemory(unsigned long size) {

    return 0;
}


X86MemoryManager::~X86MemoryManager() {

}
