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

        if (current->next == (struct memoryEntry*)NULL) return NULL;
    }

    struct memoryEntry* usedBlock = current;
    struct memoryEntry* freeBlock = current + realSize;
    H_SIZE_T usedBlockSize = realSize;
    H_SIZE_T freeBlockSize = current->size - realSize;

    if (current->size != realSize) {
        this->addToEntry(&freeEntry_, freeBlock, freeBlockSize);
        this->concatBlock(freeEntry_, freeBlock);
    }

    this->deleteFromEntry(&freeEntry_, current, current->size);
    this->addToEntry(&usedEntry_, usedBlock, usedBlockSize);

    /* return address of allocated memory */
    this->printInfo("allocate memory");
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

    struct memoryEntry* targetAddress = (struct memoryEntry*)((H_SIZE_T)address - 8);
    _sys_printf("free %d", (H_SIZE_T)address);
    this->deleteFromEntry(&usedEntry_, targetAddress, targetAddress->size);
    this->addToEntry(&freeEntry_, targetAddress, targetAddress->size);
    this->concatBlock(freeEntry_, targetAddress);
    this->printInfo("free memory");
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
    freeEntry_->next = (struct memoryEntry*)NULL;

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
void X86MemoryManager::printInfo(char* str) {

//      struct memoryEntry* entry;
//      int i;

//      for (entry = freeEntry_, i = 0; entry != (struct memoryEntry*)NULL; entry = entry->next, i++) {

//          _sys_printf("%sfree block%d address=%d size=%d\n", str, i, entry, entry->size);
//      }

//      for (entry = usedEntry_, i = 0; entry != (struct memoryEntry*)NULL; entry = entry->next, i++) {

//          _sys_printf("%sused block%d address=%d size=%d\n", str, i, entry, entry->size);

//      }

    struct memoryEntry* fentry = freeEntry_;
    struct memoryEntry* uentry = usedEntry_;
    _sys_printf("memory codition @@@@@@@@@ ");
    while (fentry || uentry) {

        if ((uentry && fentry > uentry) || (!fentry && uentry)) {
            _sys_printf("[U%d-%d]", uentry->size, uentry);
            uentry = uentry->next;
            continue;
        }

        if ((fentry && uentry > fentry) || (!uentry && fentry)) {
            _sys_printf("[F%d-%d]", fentry->size, fentry);
            fentry = fentry->next;
            continue;
        }
    }
    _sys_printf(" %s \n", str);
}

/*!
    \brief add block to entry

    add block to entries

    \param block block to entry
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void X86MemoryManager::addToEntry(struct memoryEntry** entry, struct memoryEntry* block, H_SIZE_T size) {

    if (*entry == (struct memoryEntry*)NULL) {

        *entry = block;
        (*entry)->size = size;
        (*entry)->next = (struct memoryEntry*)NULL;
        return;
    }

    if ((*entry) > block) {
        block->next = (*entry);
        block->size = size;
        *entry = block;
        return;
    }

    if ((*entry)->next == (struct memoryEntry*)NULL && block > (*entry)) {

        block->next = (struct memoryEntry*)NULL;
        block->size = size;
        (*entry)->next = block;
        return;
    }

    struct memoryEntry* current;
    struct memoryEntry* next;
    H_SIZE_T nextSize;

    for (current = (*entry); current != (struct memoryEntry*)NULL; current = current->next) {

        if (current->next > block) {
            next = current->next;
            current->next = block;
            block->size = size;
            block->next = next;
            return;
        }

        if (current->next == (struct memoryEntry*)NULL) {
            current->next = block;
            block->next = (struct memoryEntry*)NULL;
            block->size = size;
            return;
        }
    }

    return;
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
void X86MemoryManager::deleteFromEntry(struct memoryEntry** entry, struct memoryEntry* block, H_SIZE_T size) {

    /* delete block is top of the list */
    if (*entry == block && (*entry)->next == *entry) {
        *entry = (struct memoryEntry*)NULL;
        return;
    } else if (*entry == block && (*entry)->next != *entry) {

        struct memoryEntry* nextNextEntry = (*entry)->next->next;
        H_SIZE_T nextSize = (*entry)->next->size;
        *entry = (*entry)->next;
        (*entry)->size = nextSize;
        (*entry)->next = nextNextEntry;
        return;
    }

    /* iterate list and find blockt to delete */
    struct memoryEntry* current;
    for (current = (*entry); current != (struct memoryEntry*)NULL; current = current->next) {

        /* block to delete found */
        if (current->next == block) {
            current->next = current->next->next;
            break;
        }
    }
    return;
}

/*!
    \brief try to concat block

    try to concat block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void X86MemoryManager::concatBlock(struct memoryEntry* entry, struct memoryEntry* block) {
}
