/*!
    \file  IA32MemoryManager.h
    \brief class IA32MemoryManager

    class MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
#ifndef _MONA_IA32MEMORYMANAGER_
#define _MONA_IA32MEMORYMANAGER_

#include<monaTypes.h>
#include<monaIo.h>
#include<monaKernel.h>
#include<monaVga.h>

/*!
    \struct memory entry struct
    \brief  startAdress is the adress of allocated memory
 */
typedef struct memoryEntry {
    struct memoryEntry* next;
    size_t size;
    byte startAddress[0];
};

/*!
    \struct page table entry
 */
typedef struct  {
    unsigned present:1;
    unsigned readWrite:1;
    unsigned user:1;
    unsigned pageWriteThrough:1;
    unsigned pageCacheDisable:1;
    unsigned Accesse:1;
    unsigned dirty:1;
    unsigned intelReserved1:1;
    unsigned intelReserved2:1;
    unsigned monaAvailable:3;
    unsigned pageBaseAddress:20;
} PTE;


/*!
    memory management class
    single pattern  applyes the instance of this class
*/
class IA32MemoryManager {

  private:
    IA32MemoryManager();
    ~IA32MemoryManager();
    IA32MemoryManager(const IA32MemoryManager&);
    IA32MemoryManager& operator = (const IA32MemoryManager&);
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

    /*!
        \brief enable A20

        enable A20 line
        over 1MB memory access enabled

        \author HigePon
        \date   create:2002/09/06 update:2002/12/25
    */
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

    /*!
        \brief get the instance of this class

        get the instance of this class
        this class has no public constructor.

        \author HigePon
        \date   create:2002/09/06 update:2002/12/25
    */
    static IA32MemoryManager& instance() {
        static IA32MemoryManager theInstance;
        return theInstance;
    }
};


#endif
