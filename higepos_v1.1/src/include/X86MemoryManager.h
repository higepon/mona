/*!
    \file  X86MemoryManager.h
    \brief class x86MemoryManager

    class MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
#ifndef _HIGEPOS_X86MEMORYMANAGER_
#define _HIGEPOS_X86MEMORYMANAGER_

#include<higeTypes.h>

/*!
    struct for memory management
    startAdress is the adress of allocated memory
 */
typedef struct memoryEntry {
    struct memoryEntry* next;
    H_SIZE_T size;
    H_BYTE startAddress[0];
};

/*!
    memory management class
    single pattern  applyes the instance of this class
*/
class X86MemoryManager {

  private:
    X86MemoryManager();
    ~X86MemoryManager();
    X86MemoryManager(const X86MemoryManager&);
    X86MemoryManager& operator = (const X86MemoryManager&);
    H_SIZE_T getRealSize(H_SIZE_T) const;
    void addToEntry(struct memoryEntry**, struct memoryEntry*, H_SIZE_T);
    void deleteFromEntry(struct memoryEntry**, struct memoryEntry*, H_SIZE_T);
    void concatBlock(struct memoryEntry*, struct memoryEntry*);
    void enableA20() const;
    const H_SIZE_T MEMORY_START;
    const H_SIZE_T MEMORY_END;
    struct memoryEntry* freeEntry_;
    struct memoryEntry* usedEntry_;
  public:

    char* getName() const;
    void* allocateMemory(H_SIZE_T);
    void freeMemory(void*);
    void printInfo(char*) const;
    static X86MemoryManager& instance() {
        static X86MemoryManager theInstance;
        return theInstance;
    }
};


#endif
