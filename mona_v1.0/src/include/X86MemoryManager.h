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
#ifndef _MONA_X86MEMORYMANAGER_
#define _MONA_X86MEMORYMANAGER_

#include<monaTypes.h>
#include<monaIo.h>
#include<monaKernel.h>
#include<monaVga.h>

/*!
    struct for memory management
    startAdress is the adress of allocated memory
 */
typedef struct memoryEntry {
    struct memoryEntry* next;
    size_t size;
    byte startAddress[0];
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
    size_t getRealSize(size_t) const;
    void addToEntry(struct memoryEntry**, struct memoryEntry*, size_t);
    void deleteFromEntry(struct memoryEntry**, struct memoryEntry*, size_t);
    void concatBlock(struct memoryEntry*, struct memoryEntry*);
    const size_t MEMORY_START;
    const size_t MEMORY_END;
    struct memoryEntry* freeEntry_;
    struct memoryEntry* usedEntry_;
  public:

    char* getName() const;
    void* allocateMemory(size_t);
    void freeMemory(void*);
    void printInfo(char*) const;
    static void enableA20() {
        _sysLock();

        /* Wait until the keyboard buffer is empty */
        /* disable keyboard                        */
        while (inportb(0x64) & 2);
        outportb(0x64, 0xAD);

        /* read output port */
        while (inportb(0x64) & 2);
        outportb(0x64, 0xD0);

        /* Save byte from input port */
        while (!(inportb(0x64) & 1));
        byte incode = inportb(0x60);

        /*  Write output port  */
        while (inportb(0x64) & 2);
        outportb(0x64, 0xD1);

        /*  GATE A20 to ON */
        while (inportb(0x64) & 2);
        outportb(0x60, (incode | 2));

        /* enable keyboard */
        while (inportb(0x64) & 2);
        outportb(0x64, 0xAE);

        /* Wait until the keyboard buffer is empty */
        while (inportb(0x64) & 2);
        _sysUnlock();

        _sys_printf("enable A20 done!\n");
        return;
    }
    static X86MemoryManager& instance() {
        static X86MemoryManager theInstance;
        return theInstance;
    }
};


#endif
