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

#include <sys/types.h>

namespace MonAPI {

typedef struct memoryentry {
    struct memoryentry* next;
    dword size;
    dword magic;
    char startAddress[0];
} MemoryEntry;

class MemoryManager {

  public:
    MemoryManager();
    ~MemoryManager();

  public:
    void initialize(dword size, dword end);
    void free(void* address);
    void* allocate(dword size);
    dword getFreeMemorySize() const;
    dword getUsedMemorySize() const;
    void debugPrint() const;
    static dword getPhysicalMemorySize();

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

typedef struct MemoryHeader {
    struct MemoryHeader* next;
    dword size;
};

class MemoryManager2 {

  public:
    MemoryManager2();
    ~MemoryManager2();

  public:
    void initialize(dword size, dword end);
    void free(void* address);
    void* allocate(dword size);
    dword getFreeMemorySize() const;
    dword getUsedMemorySize() const;
    static dword getPhysicalMemorySize();

  private:
    MemoryHeader* freeList_;
    dword start_;
    dword end_;

};

}

#endif
