/*!
    \file  MemoryManager.h
    \brief class MemoryManager

    class MemoryManager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/28 update:$Date$
*/
#ifndef _MONA_MEMORY_MANAGER_
#define _MONA_MEMORY_MANAGER_

#include <types.h>

typedef struct memoryentry {
    struct memoryentry* next;
    dword size;
    char startAddress[0];
} MemoryEntry;

class MemoryManager {

  public:
    MemoryManager();
    MemoryManager(const MemoryManager&);
    ~MemoryManager();

  public:
    dword initialize(dword size, dword end);
    dword allocate(dword size);
    void free(dword address);
    dword getFreeMemorySize() const;
    dword getUsedMemorySize() const;
    void debugPrint() const;
    static MemoryManager& instance();

  private:
    bool hasNoEntry(MemoryEntry* list) const;
    void addToList(MemoryEntry** list, MemoryEntry* entry);
    void deleteFromList(MemoryEntry** list, MemoryEntry* entry);
    void addToNext(MemoryEntry* current, MemoryEntry* next);
    void concatFreeList();
    bool tryConcat(MemoryEntry* entry);
    dword getRealSize(dword size);

  private:
    MemoryEntry* freeList_;
    MemoryEntry* usedList_;
    dword start_;
    dword end_;

};

#endif
