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

#define GD_NUM 8192 /*!< number of Descripter */

#pragma pack(2)

/*!
    \struct gdtr
*/
typedef struct {
    word  limit;
    dword base;
} GDTR;

#pragma pack()

/*!
    \struct  gdt
*/
typedef struct {
    word limitL;
    word baseL;
    byte baseM;
    byte type;
    byte limitH;
    byte baseH;
} GDT;

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

/*! Descriptor Type */
#define TypeCode     0x9a
#define TypeData     0x92
#define TypeStack    0x96
#define TypeLDT      0x82
#define TypeTSS      0x89
#define TypeTSSBusy  0x8b
#define TypeCallGate 0x84
#define TypeIntrGate 0x8e
#define TypeTrapGate 0x8f
#define TypeTaskGate 0x85

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
    struct memoryEntry* freeEntry_;
    struct memoryEntry* usedEntry_;

    GDT gdt_[8192]; /*!< Global Descriptor table */

  public:

    char* getName() const;
    void* allocateMemory(size_t);
    void freeMemory(void*);
    inline void printInfo(char*) const;
    static void enableA20();
    static IA32MemoryManager& instance();
    static void startPaging();
    static void stopPaging();
    void setGDT(word, dword, dword, byte, byte, byte);
    void resetGDT();
    /* memo setWriter(write);setReader(reader); */

};


#endif
