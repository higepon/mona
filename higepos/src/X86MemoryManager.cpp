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
    \date   create:2002/08/07 update:2002/09/08
*/
void* X86MemoryManager::allocateMemory(H_SIZE_T size) {

    /* size 0 */
    if (size == 0) return NULL;

    /* there is no free list */
    if (freeEntry_ == NULL) return NULL;

    /* getRealSize */
    H_SIZE_T realSize = this->getRealSize(size);

    /* search block in free list that has enough size for needed */
    struct memoryEntry* current;
    for (current = freeEntry_; ; current = current->next) {

        if (current->size >= realSize) break;

        if(current->next == freeEntry_) return NULL;
    }

    _sys_printf("size=%d realSize=%d current= %d\n", size, realSize, current);
    struct memoryEntry* usedBlock = current;
    struct memoryEntry* freeBlock = current + realSize;
    H_SIZE_T usedBlockSize = realSize;
    H_SIZE_T freeBlockSize = current->size - realSize;

    if (current->size != realSize) {
        this->addToEntry(0, freeBlock, freeBlockSize);
        this->concatBlock(freeEntry_, freeBlock);
    }
    this->deleteFromEntry(freeEntry_, current, current->size);
    _sysPrintln("**************");
    _sysPrintlnInt((int)usedBlock);
    this->addToEntry(1, usedBlock, usedBlockSize);

    /* address of allocated memory */
    this->printInfo();
    return (void*)usedBlock->startAddress;
}

/*!
    \brief free memory

    free memory

    \param address adrress of memory to be free
    \return result

    \author HigePon
    \date   create:2002/08/07 update:
*/
void X86MemoryManager::freeMemory(void* address) {

    struct memoryEntry* targetAddress = (struct memoryEntry*)address;

    this->deleteFromEntry(usedEntry_, targetAddress, targetAddress->size);
    this->addToEntry(0, targetAddress, targetAddress->size);
    this->concatBlock(freeEntry_, targetAddress);
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
    \date   create:2002/09/07 update:2002/09/08
*/
void X86MemoryManager::printInfo() {

    struct memoryEntry* temp = freeEntry_;

    for (int i = 0; freeEntry_; i++, temp = temp->next) {

        _sys_printf("free block%d address=%d size=%d\n", i, temp, temp->size);
        if (freeEntry_ == temp->next) break;
    }

    temp = usedEntry_;
    for (int i = 0; usedEntry_; i++, temp = temp->next) {

        _sys_printf("used block%d address=%d size=%d\n", i, temp, temp->size);
        if (usedEntry_ == temp->next) break;
    }
}

/*!
    \brief add block to entry

    add block to entries

    \param type  0 FREE: 1 USED
    \param block block to entry
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void X86MemoryManager::addToEntry(H_BYTE type, struct memoryEntry* block, H_SIZE_T size) {

    struct memoryEntry* entry    = type? usedEntry_ : freeEntry_;
    struct memoryEntry* previous = (struct memoryEntry*)NULL;
    struct memoryEntry* current  = entry;

    if (type && usedEntry_ == (struct memoryEntry*)NULL) {

        usedEntry_ = block;
        usedEntry_->size = size;
    }

    for (; ; previous = current, current = previous->next) {

        /* the add position */
        if (block >= current) break;

        /* block not found */
        if (current->next == entry) return;
    }

    struct memoryEntry* next = current->next;

    if (previous) {
        previous->next = block;
        block->next    = next;
        block->size    = size;
    } else {
        entry->size = size;
    }
}

/*!
    \brief delete from entry

    delete from entry

    \param entry freeEntry of usedEntry
    \param block block to delete
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::deleteFromEntry(struct memoryEntry* entry, struct memoryEntry* block, H_SIZE_T size) {

    struct memoryEntry* previous = (struct memoryEntry*)NULL;
    struct memoryEntry* current  = entry;
    for (; ; previous = current, current = previous->next) {

        /* the add position */
        if (block == current) break;

        /* block not found */
        if (current == entry) return;
    }

    struct memoryEntry* next  = current->next;
    previous->next = next;
}

/*!
    \brief try to concat block

    try to concat block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::concatBlock(struct memoryEntry* entry, struct memoryEntry* block) {
}
