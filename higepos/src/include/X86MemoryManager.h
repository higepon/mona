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

#include<higepostypes.h>

/*!
    struct for memory management
    startAdress is the adress of allocated memory
 */
struct memoryEntry {
    struct memoryEntry* next;
    int size;
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
    H_SIZE_T getRealSize(H_SIZE_T size);
    const H_SIZE_T MEMORY_START;
    const H_SIZE_T MEMORY_END;
    H_SIZE_T current_;
    struct memoryEntry* entry_;
  public:

    char* getName();
    H_SIZE_T allocateMemory(H_SIZE_T);
    H_SIZE_T freeMemory(H_SIZE_T);
    static X86MemoryManager& instance() {
        static X86MemoryManager theInstance;
        return theInstance;
    }
};


#endif
