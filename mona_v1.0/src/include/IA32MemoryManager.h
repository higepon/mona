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

/*! Type */
#define TYPE_RESERVED1   0x0
#define TYPE_TSS16       0x1
#define TYPE_LDT         0x2
#define TYPE_TSS_BUSY16  0x3
#define TYPE_CALL_GATE16 0x4
#define TYPE_TASK_GATE   0x5
#define TYPE_INTR_GATE16 0x6
#define TYPE_TRAP_GATE16 0x7
#define TYPE_RESERVED2   0x8
#define TYPE_TSS         0x9
#define TYPE_RESERVED3   0xA
#define TYPE_TSS_BUSY    0xB
#define TYPE_CALL_GATE   0xC
#define TYPE_RESERVED4   0xD
#define TYPE_INTR_GATE   0xE
#define TYPE_TRAP_GATE   0xF

/*! Descriptor Type */
#define SYSTEM_SEGMENT 0x00
#define CODE_OR_DATA   0x10

/* Descriptor Privilege Level */
#define DPL0 0x00
#define DPL1 0x20
#define DPL2 0x40
#define DPL3 0x60

/* Present */
#define SEGMENT_ABSENT  0x00
#define SEGMENT_PRESENT 0x80

/* TYPE SUM */
#define SYS_CODE     (byte)(SEGMENT_PRESENT | DPL0 | CODE_OR_DATA   | 0xA          )
#define SYS_DATA     (byte)(SEGMENT_PRESENT | DPL0 | CODE_OR_DATA   | 0x2          )
#define SYS_STACK    (byte)(SEGMENT_PRESENT | DPL0 | CODE_OR_DATA   | 0x6          )
#define SYS_TSS      (byte)(SEGMENT_PRESENT | DPL0 | SYSTEM_SEGMENT | TYPE_TSS     )
#define SYS_TSS_BUSY (byte)(SEGMENT_PRESENT | DPL0 | SYSTEM_SEGMENT | TYPE_TSS_BUSY)
#define SYS_LDT      (byte)(SEGMENT_PRESENT | DPL0 | SYSTEM_SEGMENT | TYPE_LDT     )

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
    GDT* gdt_; /*!< Global Descriptor table */

  public:

    char* getName() const;
    void* allocateMemory(size_t);
    void freeMemory(void*);
    void printInfo(char*) const;
    static void enableA20();
    static IA32MemoryManager& instance();
    static void startPaging();
    static void stopPaging();
    void setGDT(word, dword, dword, byte, byte);
    void resetGDT();
    /* memo setWriter(write);setReader(reader); */

};


#endif
