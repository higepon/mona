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

/*!
    memory management class
    the instance of this class is single pattern
*/
class X86MemoryManager {

  private:
    X86MemoryManager();
    ~X86MemoryManager();
    X86MemoryManager(const X86MemoryManager&);
    X86MemoryManager& operator = (const X86MemoryManager&);
    unsigned long start_;
  public:

    char* getName();
    unsigned long allocateMemory(unsigned long);
    unsigned long freeMemory(unsigned long);
    static X86MemoryManager& instance() {
        static X86MemoryManager theInstance;
        return theInstance;
    }
};

#endif
