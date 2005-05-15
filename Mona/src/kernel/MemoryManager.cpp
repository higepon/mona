/*!
    \file   MemoryManger.cpp
    \brief  MemoryMangement allocate and free

    Copyright (c) 2002,2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    Al rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/28 update:$Date$
*/

#include "MemoryManager.h"
#include "global.h"

#define SIZE_OF_HEADER sizeof(MemoryEntry)

MemoryManager::MemoryManager() {
}

void MemoryManager::initialize(dword start, dword end) {

    /* create large free block */
    freeList_       = (MemoryEntry*)start;
    freeList_->next = (MemoryEntry*)NULL;
    freeList_->size = end - start - SIZE_OF_HEADER;

    /* memory not used yet */
    usedList_ = (MemoryEntry*)NULL;

    /* start end */
    start_ = start;
    end_   = end;
}

MemoryManager::~MemoryManager() {
}

bool MemoryManager::hasNoEntry(MemoryEntry* list) const {

    return (list == (MemoryEntry*)NULL);
}

void MemoryManager::addToNext(MemoryEntry* current, MemoryEntry* next) {

    MemoryEntry* nextnext = current->next;
    current->next         = next;
    next->next            = nextnext;
}

void MemoryManager::addToList(MemoryEntry** list, MemoryEntry* entry) {

    MemoryEntry* target = *list;

    if (hasNoEntry(target)) {

        *list        = entry;
        entry->next  = (MemoryEntry*)NULL;
        return;
    }

    if (entry < target) {

        entry->next = target;
        *list = entry;
        return;
    }

    for (; ; target = target->next) {

        bool isBetween   = target < entry && entry < target->next;
        bool isEndOfList = target->next == (MemoryEntry*)NULL;

        if (isBetween || isEndOfList) {

            addToNext(target, entry);
            break;
        }
    }

    return;
}

void MemoryManager::concatFreeList() {

    for (MemoryEntry* current = freeList_; current != (MemoryEntry*)NULL; current = current->next) {

        while (true) {
            if (!tryConcat(current)) break;
        }
    }
}

bool MemoryManager::tryConcat(MemoryEntry* entry) {

    /* has no next entry */
    if (entry->next == (MemoryEntry*)NULL) return false;

    MemoryEntry* expectedNext = (MemoryEntry*)((dword)entry + getRealSize(entry->size));

    /* can not concat */
    if (expectedNext != entry->next) return false;

    /* concat */
    dword nextSize = getRealSize(entry->next->size);
    entry->next = entry->next->next;
    entry->size += nextSize;

    return true;
}

dword MemoryManager::getRealSize(dword size) {

    return (size + SIZE_OF_HEADER);
}

void* MemoryManager::allocate(dword size) {

    if (size == 0) return (dword)NULL;

    /* align16 */
    size = (size + 16 - 1) & 0xFFFFFFF0;

    MemoryEntry* current;
    dword realSize = getRealSize(size);

    for (current = freeList_; ; current = current->next) {

        if (current->size >= realSize) break;
        if (current->next == (MemoryEntry*)NULL) {
            return (dword)NULL;
        }
    }

    if (current->size == realSize) {

        deleteFromList(&freeList_, current);
        addToList(&usedList_, current);
        memset(current->startAddress, 0, size);

#ifdef DEBUG_BUILD
        current->pad1 = 0xFBFBFBFB;
        current->pad2 = 0xFBFBFBFB;
//      if (((int)(current->startAddress)) % 16) panic("16");
#endif
        return (current->startAddress);

    } else if (current->size >= realSize) {

        MemoryEntry* freeBlock = (MemoryEntry*)((dword)current + realSize);
        freeBlock->size  = getRealSize(current->size) - realSize - SIZE_OF_HEADER;
        current->size = size;

        deleteFromList(&freeList_, current);
        addToList(&usedList_, current);
        addToList(&freeList_, freeBlock);
        memset(current->startAddress, 0, size);

#ifdef DEBUG_BUILD
        current->pad1 = 0xFBFBFBFB;
        current->pad2 = 0xFBFBFBFB;
//      if (((int)(current->startAddress)) % 16) panic("16");
#endif

        return (current->startAddress);

    } else {
        return (dword)NULL;
    }
}

void MemoryManager::free(void* address) {

    dword target = (dword)address;

    if (target < 1) return;

    MemoryEntry* entry = (MemoryEntry*)(target - SIZE_OF_HEADER);

#ifdef DEBUG_BUILD
    if (entry->pad1 != 0xFBFBFBFB || entry->pad2 != 0xFBFBFBFB)
    {
        panic("Memory Manager, error");
    }
#endif

    deleteFromList(&usedList_, entry);
    addToList(&freeList_, entry);
    concatFreeList();
}

void MemoryManager::deleteFromList(MemoryEntry** list, MemoryEntry* entry) {

    /* delete block is top of the list */
    if (*list == entry && (*list)->next == (MemoryEntry*)NULL) {

        *list = (MemoryEntry*)NULL;
        return;
    } else if (*list == entry && (*list)->next != (MemoryEntry*)NULL) {

        MemoryEntry* next = (*list)->next;
        *list = next;
        return;
    }

    /* iterate list and find block to delete */
    for (MemoryEntry* current = *list; current != (MemoryEntry*)NULL; current = current->next) {

        /* block to delete found */
        if (current->next == entry) {
            current->next = current->next->next;
            break;
        }
    }

    return;
}

dword MemoryManager::getFreeMemorySize() const {

    dword result = 0;

    for (MemoryEntry* current = freeList_; current != (MemoryEntry*)NULL; current = current->next) {

        result += (current->size);
    }
    return result;
}

dword MemoryManager::getUsedMemorySize() const {

    dword result = 0;

    for (MemoryEntry* current = usedList_; current != (MemoryEntry*)NULL; current = current->next) {

        result += (current->size);
    }
    return result;
}

void MemoryManager::debugPrint() const {

//     printf("Rage[%x-%x]\n", start_, end_);
//     printf("FreeMemorySize=%x\n", getFreeMemorySize());

//     for (MemoryEntry* current = freeList_; current != (MemoryEntry*)NULL; current = current->next) {

//         printf("F[%x][%x]\n", current, current->size);
//     }

//     printf("UsedMemorySize=%x\n", getUsedMemorySize());

//     for (MemoryEntry* current = usedList_; current != (MemoryEntry*)NULL; current = current->next) {

//         printf("U[%x][%x]\n", current, current->size);
//     }
}

dword MemoryManager::getPhysicalMemorySize() {

    /* assume there is at least 1MB memory */
    dword totalMemorySize  = 1024 * 1024;

    /* 1MB unit loop */
    for (dword i = 1024 * 1024; i < 0xFFFFFFFF; i += 1024 * 1024) {

        volatile dword* p = (dword*)i;
        dword value = *p;

        *p = 0x12345678;
        if (*p != 0x12345678) break;

        *p = value;
        totalMemorySize += 1024 * 1024;
    }

    return totalMemorySize;
}
