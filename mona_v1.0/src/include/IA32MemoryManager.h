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

#include<types.h>
#include<kernel.h>

/*!
    \struct memoryEntry
    \brief  startAdress is the adress of allocated memory
 */
typedef struct memoryEntry {
    struct memoryEntry* next;
    size_t size;
    byte startAddress[0];
};

/*!
    \struct PTE
 */
typedef struct  {
    unsigned present:1;          /*!< present bit            */
    unsigned readWrite:1;        /*!< read/write bit         */
    unsigned user:1;             /*!< user superviser bit    */
    unsigned pageWriteThrough:1; /*!< page write through bit */
    unsigned pageCacheDisable:1; /*!< page cache disable bit */
    unsigned accesse:1;          /*!< access bit             */
    unsigned dirty:1;            /*!< dirty bit              */
    unsigned intelReserved:2;    /*!< reserved by intel      */
    unsigned monaAvailable:3;    /*!< mona may use this      */
    unsigned pageBaseAddress:20; /*!< base address of page   */
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
    inline void setCR3(dword) const;
    inline void flushTLB() const;
    inline void lgdt() const;
    const size_t MEMORY_START;
    const size_t MEMORY_END;
    dword usedMemorySize_;
    struct memoryEntry* freeEntry_;
    struct memoryEntry* usedEntry_;

  public:

    char* getName() const;
    void* allocateMemory(size_t);
    void freeMemory(void*);
    dword getTotalMemory();
    dword getUsedMemory();
    void printInfo(char*) const;
    static IA32MemoryManager& instance();
    static void startPaging();
    static void stopPaging();
    /* memo setWriter(write);setReader(reader); */

};


#endif
