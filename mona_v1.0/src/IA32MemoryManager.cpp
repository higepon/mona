/*!
    \file  IA32MemoryManager.cpp
    \brief class IA32MemoryManager

    class IA32MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/04 update:$Date$
*/
#include<IA32MemoryManager.h>
#include<monaIdt.h>
#include<monaVga.h>
#include<monaKernel.h>
#include<monaOperator.h>

/*!
    \brief get class Name

    get class Name

    \author HigePon
    \date   create:2002/08/04 update:
*/
inline char* IA32MemoryManager::getName() const {

    static char buf[] = "IA32MemoryManager";
    return buf;
}

/*!
    \brief allocate memory

    allocate memory

    \param size size of memory to allocate
    \return address to allocated memory

    \author HigePon
    \date   create:2002/08/07 update:2002/09/08
*/
void* IA32MemoryManager::allocateMemory(size_t size) {

    /* size 0 */
    if (size == 0) return NULL;

    /* there is no free list */
    if (freeEntry_ == NULL) return NULL;

    /* getRealSize */
    size_t realSize = this->getRealSize(size);

    /* search block in free list that has enough size for needed */
    struct memoryEntry* current;
    for (current = freeEntry_; ; current = current->next) {

        if (current->size >= realSize) break;

        if (current->next == (struct memoryEntry*)NULL) return NULL;
    }

    struct memoryEntry* usedBlock = current;
    struct memoryEntry* freeBlock = current + realSize;
    size_t usedBlockSize = realSize;
    size_t freeBlockSize = current->size - realSize;

    if (current->size != realSize) {
        this->addToEntry(&freeEntry_, freeBlock, freeBlockSize);
        this->concatBlock(freeEntry_, freeBlock);
    }

    this->deleteFromEntry(&freeEntry_, current, current->size);
    this->addToEntry(&usedEntry_, usedBlock, usedBlockSize);

#ifdef MONA_DEBUG_MM
    this->printInfo("allocate memory");
#endif

    /* return address of allocated memory */
    return (void*)usedBlock->startAddress;
}

/*!
    \brief free memory

    free memory

    \param address adrress of memory to be free
    \return result

    \author HigePon
    \date   create:2002/08/07 update:
*/
void IA32MemoryManager::freeMemory(void* address) {

    struct memoryEntry* targetAddress = (struct memoryEntry*)((size_t)address
                                      - sizeof(size_t)
                                      - sizeof(struct memoryEntry*));
    this->deleteFromEntry(&usedEntry_, targetAddress, targetAddress->size);
    this->addToEntry(&freeEntry_, targetAddress, targetAddress->size);
    this->concatBlock(freeEntry_, targetAddress);

#ifdef MONA_DEBUG_MM
    this->printInfo("free memory");
#endif

    return;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/08/08 update:
*/
IA32MemoryManager::~IA32MemoryManager() {
    _sys_printf("IA32MemoryManager:destructor\n");

}

/*!
    \brief private constructor

    constructor

    set up list of free memory,determin size and range of memory

    \author HigePon
    \date   create:2002/08/10 update:2003/01/03
*/
IA32MemoryManager::IA32MemoryManager():MEMORY_START(0x15000), MEMORY_END(0x150000) {

    /* first time, the number of free memory list is one. */
    freeEntry_ = (struct memoryEntry*)MEMORY_START;
    freeEntry_->size = MEMORY_END - MEMORY_START;
    freeEntry_->next = (struct memoryEntry*)NULL;

    /* there is no usedEntry */
    usedEntry_ = (struct memoryEntry*)NULL;
    return;
}

/*!
    \brief get real size

    get real size of memory to allocate.
    size of struct MemoryEntry should be added

    \param size size of memory they want
    \return real size of memory to allocate

    \author HigePon
    \date   create:2002/09/07 update:
*/
inline size_t IA32MemoryManager::getRealSize(size_t size) const {

    return (size + sizeof(struct memoryEntry));
}

/*!
    \brief print info

    print memory information for debug

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void IA32MemoryManager::printInfo(char* str) const {

//      struct memoryEntry* entry;
//      int i;

//      for (entry = freeEntry_, i = 0; entry != (struct memoryEntry*)NULL; entry = entry->next, i++) {

//          _sys_printf("%sfree block%d address=%d size=%d\n", str, i, entry, entry->size);
//      }

//      for (entry = usedEntry_, i = 0; entry != (struct memoryEntry*)NULL; entry = entry->next, i++) {

//          _sys_printf("%sused block%d address=%d size=%d\n", str, i, entry, entry->size);

//      }

    struct memoryEntry* fentry = freeEntry_;
    struct memoryEntry* uentry = usedEntry_;

    while (fentry || uentry) {

        if ((uentry && fentry > uentry) || (!fentry && uentry)) {
            _sys_printf("[U%x-%x]", uentry->size, uentry);
            uentry = uentry->next;
            continue;
        }

        if ((fentry && uentry > fentry) || (!uentry && fentry)) {
            _sys_printf("[F%x-%x]", fentry->size, fentry);
            fentry = fentry->next;
            continue;
        }
    }
    _sys_printf(" %s \n", str);
    return;
}

/*!
    \brief add block to entry

    add block to entries

    \param entry free or used
    \param block block to add
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:2002/09/08
*/
void IA32MemoryManager::addToEntry(struct memoryEntry** entry, struct memoryEntry* block, size_t size) {

    if (*entry == (struct memoryEntry*)NULL) {

        *entry = block;
        (*entry)->size = size;
        (*entry)->next = (struct memoryEntry*)NULL;
        return;
    }

    if ((*entry) > block) {
        block->next = (*entry);
        block->size = size;
        *entry = block;
        return;
    }

    if ((*entry)->next == (struct memoryEntry*)NULL && block > (*entry)) {

        block->next = (struct memoryEntry*)NULL;
        block->size = size;
        (*entry)->next = block;
        return;
    }

    struct memoryEntry* current;
    struct memoryEntry* next;

    for (current = (*entry); current != (struct memoryEntry*)NULL; current = current->next) {

        if (current->next > block) {
            next = current->next;
            current->next = block;
            block->size = size;
            block->next = next;
            return;
        }

        if (current->next == (struct memoryEntry*)NULL) {
            current->next = block;
            block->next = (struct memoryEntry*)NULL;
            block->size = size;
            return;
        }
    }

    return;
}

/*!
    \brief delete from entry

    delete from entry

    \param entry freeEntry of usedEntry
    \param block block to delete
    \param size  size of block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void IA32MemoryManager::deleteFromEntry(struct memoryEntry** entry, struct memoryEntry* block, size_t size) {

    /* delete block is top of the list */
    if (*entry == block && (*entry)->next == (struct memoryEntry*)NULL) {
        *entry = (struct memoryEntry*)NULL;
        return;
    } else if (*entry == block && (*entry)->next != (struct memoryEntry*)NULL) {

        struct memoryEntry* nextNextEntry = (*entry)->next->next;
        size_t nextSize = (*entry)->next->size;
        *entry = (*entry)->next;
        (*entry)->size = nextSize;
        (*entry)->next = nextNextEntry;
        return;
    }

    /* iterate list and find blockt to delete */
    struct memoryEntry* current;
    for (current = (*entry); current != (struct memoryEntry*)NULL; current = current->next) {

        /* block to delete found */
        if (current->next == block) {
            current->next = current->next->next;
            break;
        }
    }
    return;
}

/*!
    \brief try to concat block

    try to concat block

    \author HigePon
    \date   create:2002/09/07 update:
*/
void IA32MemoryManager::concatBlock(struct memoryEntry* entry, struct memoryEntry* block) {

    /* do nothing */
    if (block == (struct memoryEntry*)NULL) return;
    if (block->next == (struct memoryEntry*)NULL) return;

    struct memoryEntry* canConcatAddress = block + (block->size);     /* address of block we can concat */
    size_t nextSize;

    /* we can concat two block into one block */
    if (block->next == canConcatAddress) {

        nextSize = getRealSize(block->next->size);
        block->next = block->next->next;
        block->size = (block->size) + nextSize;
    }
    return;
}

/*!
    \brief setCR3

    set pageDirAddress to CR3 register

    \param pageDirAddress physical address of page dir table.

    \author HigePon
    \date   create:2002/12/25 update:
*/
inline void IA32MemoryManager::setCR3(dword pageDirAddress) const {

    asm volatile("movd %0, %%cr3" : /* no output */
                                  : "m" (pageDirAddress) : "ax");
}

/*!
    \brief get the instance of this class

    get the instance of this class
    this class has no public constructor.

    \author HigePon
    \date   create:2002/09/06 update:2002/12/25
*/
IA32MemoryManager& IA32MemoryManager::instance() {
    static IA32MemoryManager theInstance;
    return theInstance;
}


/*!
    \brief start paging

    start paging on

    \author HigePon
    \date   create:2002/12/25 update:
*/
void IA32MemoryManager::startPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief stop paging

    set paging off

    \author HigePon
    \date   create:2002/12/25 update:
*/
void IA32MemoryManager::stopPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x7fffffff, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief flush TLB

    flush Translation Lookaside Buffer.

    \author HigePon
    \date   create:2002/12/26 update:
*/
inline void IA32MemoryManager::flushTLB() const {

    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief set up GDT

    set up descripter in GDT

    \param index  index in GDT
    \param base   base address of segment
    \param limit  limit of segment
    \param type   segment type
    \param type32 type32

    \author HigePon
    \date   create:2002/12/31 update:
*/
void IA32MemoryManager::setGDT(word index, dword base, dword limit, byte type, byte type32) {

    /* check index */
    if (index >= GD_NUM) return;

    /* set descriptor */
    gdt_[index].baseL  = (word)(base & 0xffff);
    gdt_[index].baseM  = (byte)((base >> 16) & 0xff);
    gdt_[index].baseH  = (byte)((base >> 24) & 0xff);
    gdt_[index].limitL = (word)(limit & 0xffff);
    gdt_[index].limitH = (byte)((limit >> 16) & 0xff + type32);
    gdt_[index].type   = type;
}

/*!
    \brief lgdt

    load adress and limit to gdtr register

    \author HigePon
    \date   create:2002/12/31 update:
*/
inline void IA32MemoryManager::lgdt() const {

    GDTR gdtr;
    gdtr.base  = (dword)gdt_;
    gdtr.limit = sizeof(GDT) * GD_NUM - 1;
    asm volatile("lgdt %0\n" : /* no output */ : "m" (gdtr));
}

/*!
    \brief reset GDT

    at secondboot.asm, set up GDT temporary.
    here, set up GDT.

    \author HigePon
    \date   create:2002/12/31 update:
*/
void IA32MemoryManager::resetGDT() {

    gdt_ = (GDT*)malloc(GD_NUM * sizeof(GDT));
    if (gdt_ == NULL) panic("memory allocate error:GDT");

    setGDT(0, 0, 0, 0, 0);
    setGDT(1, 0, 0xFFFFFFFF, SYS_CODE , 0);
    setGDT(2, 0, 0xFFFFFFFF, SYS_DATA , 0);
    setGDT(3, 0, 0x00C00000, SYS_STACK, 0);
    setGDT(4, 0, 0x00000067, SYS_TSS  , 0);
    setGDT(5, 0, 0x00000067, SYS_TSS  , 0);
    setGDT(6, 0, 0x00000067, SYS_LDT  , 0);

    /* system code segment for task switch */
    setGDT(7, 0, 0xFFFFFFFF, SYS_CODE , 0);
    lgdt();
    return;
}
