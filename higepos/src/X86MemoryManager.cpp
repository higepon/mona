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
#include<idt.h>
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
    \date   create:2002/08/07 update:2002/09/07
*/
H_SIZE_T X86MemoryManager::allocateMemory(H_SIZE_T size) {

    /* size 0 */
    if (size == 0) return NULL;

    H_SIZE_T oldAddress = current_;
    current_ += size;

    /* check limit */
    if (current_ > MEMORY_END) {
        oldAddress = MEMORY_START;
        current_   = MEMORY_START;
        fault0dHandler();
    }

    /* adress of allocated memory */
    return oldAddress;
}

/*!
    \brief free memory

    free memory

    \param address adrress of memory to be free
    \return result

    \author HigePon
    \date   create:2002/08/07 update:
*/
H_SIZE_T X86MemoryManager::freeMemory(H_SIZE_T address) {

    return 0;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/08/08 update:
*/
X86MemoryManager::~X86MemoryManager() {
}

/*!
    \brief private constructor

    constructor

    \author HigePon
    \date   create:2002/08/10 update:2002/08/24
*/
X86MemoryManager::X86MemoryManager():MEMORY_START(0x10000), MEMORY_END(0x15000) {
    current_ = MEMORY_START;
}

/*!
    \brief get real size

    get real size of memory to allocate.
    size of struct MemoryEntry should be added

    \param size size of memory they want
    \return real size of memory to allocate

    \author HigePon
    \date   create:2002/09/07 update:
*/
H_SIZE_T X86MemoryManager::getRealSize(H_SIZE_T size) {

    return (size + sizeof(struct memoryEntry));
}
