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
#include<vga.h>
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

    /* there is no free list */
    if (freeEntry_ == NULL) return NULL;

    /* getRealSize */
    H_SIZE_T realSize = this->getRealSize(size);
    _sysPrint("real size is");
    _sysPrintlnInt((int)realSize);

    /* search block in free list that has enough size for needed */
    struct memoryEntry* previous = (struct memoryEntry*)NULL;
    struct memoryEntry* current  = freeEntry_;
    for (; ; previous = current, current = previous->next) {

        /* first fit found */
        if (current->size > realSize) break;

        /* block not found */
        if (current == freeEntry_) return NULL;
    }

    struct memoryEntry* usedBlock = current;
    struct memoryEntry* freeBlock = current + realSize;
    H_SIZE_T usedBlockSize = realSize;
    H_SIZE_T freeBlockSize = current->size + realSize;

    this->addToEntry(usedEntry_, usedBlock, usedBlockSize);
    this->addToEntry(freeEntry_, freeBlock, freeBlockSize);

    if (current->size == realSize) {

    }

    /* split the found free block into used and free */


    /* regist to used list */

    /* regist to free list */


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

    set up list of free memory,determin size and range of memory

    \author HigePon
    \date   create:2002/08/10 update:2002/09/07
*/
X86MemoryManager::X86MemoryManager():MEMORY_START(0x10000), MEMORY_END(0x15000) {
    current_ = MEMORY_START;

    /* first time, the number of free memory list is one. */
    freeEntry_ = (struct memoryEntry*)MEMORY_START;
    freeEntry_->size = MEMORY_END - MEMORY_START;
    freeEntry_->next = freeEntry_;

    /* there is no usedEntry */
    usedEntry_ = (struct memoryEntry*)NULL;
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

/*!
    \brief print info

    print memory information for debug

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::printInfo() {

    _sysPrintln("X86MemoryManager Information");

}

/*!
    \brief add block to entry

    add block to entries

    \param entry freeEntry of usedEntry
    \param block block to entry
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::addToEntry(struct memoryEntry* entry, struct memoryEntry* block, H_SIZE_T size) {
    _sysPrintln("X86MemoryManager Information");
}

/*!
    \brief try to concat block

    try to concat block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::concatBlock(struct memoryEntry* block) {
    _sysPrintln("X86MemoryManager Information");
}
