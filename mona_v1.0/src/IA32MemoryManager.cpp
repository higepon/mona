/*!
    \file  IA32MemoryManager.cpp
    \brief class IA32MemoryManager

    class IA32MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
#include<IA32MemoryManager.h>
#include<monaIdt.h>
#include<monaVga.h>

/*!
    \brief get class Name

    get class Name

    \author HigePon
    \date   create:2002/08/04 update:
*/
inline char* IA32MemoryManager::getName() const {

    static char buf[] = "IA32MemoryManager";
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
void* IA32MemoryManager::allocateMemory(size_t size) {

    /* size 0 */
    if (size == 0) return NULL;

    /* there is no free list */
    if (freeEntry_ == NULL) return NULL;

    /* getRealSize */
    size_t realSize = this->getRealSize(size);

    /* search block in free list that has enough size for needed */
    struct memoryEntry* current;
    for (current = freeEntry_; ; current = current->next) {

        if (current->size >= realSize) break;

        if (current->next == (struct memoryEntry*)NULL) return NULL;
    }

    struct memoryEntry* usedBlock = current;
    struct memoryEntry* freeBlock = current + realSize;
    size_t usedBlockSize = realSize;
    size_t freeBlockSize = current->size - realSize;

    if (current->size != realSize) {
        this->addToEntry(&freeEntry_, freeBlock, freeBlockSize);
        this->concatBlock(freeEntry_, freeBlock);
    }

    this->deleteFromEntry(&freeEntry_, current, current->size);
    this->addToEntry(&usedEntry_, usedBlock, usedBlockSize);

#ifdef MONA_DEBUG_MM
    this->printInfo("allocate memory");
#endif

    /* return address of allocated memory */
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
void IA32MemoryManager::freeMemory(void* address) {

    struct memoryEntry* targetAddress = (struct memoryEntry*)((size_t)address
                                      - sizeof(size_t)
                                      - sizeof(struct memoryEntry*));
    this->deleteFromEntry(&usedEntry_, targetAddress, targetAddress->size);
    this->addToEntry(&freeEntry_, targetAddress, targetAddress->size);
    this->concatBlock(freeEntry_, targetAddress);

#ifdef MONA_DEBUG_MM
    this->printInfo("free memory");
#endif

    return;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/08/08 update:
*/
IA32MemoryManager::~IA32MemoryManager() {
    _sys_printf("KeyBoardManager:destructor\n");

}

/*!
    \brief private constructor

    constructor

    set up list of free memory,determin size and range of memory

    \author HigePon
    \date   create:2002/08/10 update:2002/11/11
*/
IA32MemoryManager::IA32MemoryManager():MEMORY_START(0x10000), MEMORY_END(0x25000) {

    /* first time, the number of free memory list is one. */
    freeEntry_ = (struct memoryEntry*)MEMORY_START;
    freeEntry_->size = MEMORY_END - MEMORY_START;
    freeEntry_->next = (struct memoryEntry*)NULL;

    /* there is no usedEntry */
    usedEntry_ = (struct memoryEntry*)NULL;
    return;
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
inline size_t IA32MemoryManager::getRealSize(size_t size) const {

    return (size + sizeof(struct memoryEntry));
}

/*!
    \brief print info

    print memory information for debug

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void IA32MemoryManager::printInfo(char* str) const {

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
    return;
}

/*!
    \brief add block to entry

    add block to entries

    \param entry free or used
    \param block block to add
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void IA32MemoryManager::addToEntry(struct memoryEntry** entry, struct memoryEntry* block, size_t size) {

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
void IA32MemoryManager::deleteFromEntry(struct memoryEntry** entry, struct memoryEntry* block, size_t size) {

    /* delete block is top of the list */
    if (*entry == block && (*entry)->next == (struct memoryEntry*)NULL) {
        *entry = (struct memoryEntry*)NULL;
        return;
    } else if (*entry == block && (*entry)->next != (struct memoryEntry*)NULL) {

        struct memoryEntry* nextNextEntry = (*entry)->next->next;
        size_t nextSize = (*entry)->next->size;
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
void IA32MemoryManager::concatBlock(struct memoryEntry* entry, struct memoryEntry* block) {

    /* do nothing */
    if (block == (struct memoryEntry*)NULL) return;
    if (block->next == (struct memoryEntry*)NULL) return;

    struct memoryEntry* canConcatAddress = block + (block->size);     /* address of block we can concat */
    size_t nextSize;

    /* we can concat two block into one block */
    if (block->next == canConcatAddress) {

        nextSize = getRealSize(block->next->size);
        block->next = block->next->next;
        block->size = (block->size) + nextSize;
    }
    return;
}
