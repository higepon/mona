/*!
  \file  PageManager.cpp
  \brief class PageManager

  Copyright (c) 2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/08/23 update:$Date$
*/

#include <PageManager.h>
#include <BitMap.h>
#include <types.h>
#include <string.h>
#include <operator.h>
#include <global.h>

/* Segment Faults */
const byte Segment::FAULT_STACK_OVERFLOW;
const byte Segment::FAULT_OUT_OF_RANGE;
const byte Segment::FAULT_UNKNOWN;

/* independent from architecture */
const byte PageManager::FAULT_NOT_EXIST;
const byte PageManager::FAULT_NOT_WRITABLE;

/* depend on architecture */
const byte PageManager::ARCH_FAULT_NOT_EXIST;
const byte PageManager::ARCH_FAULT_ACCESS_DENIED;
const byte PageManager::ARCH_FAULT_READ;
const byte PageManager::ARCH_FAULT_WRITE;
const byte PageManager::ARCH_FAULT_WHEN_KERNEL;
const byte PageManager::ARCH_FAULT_WHEN_USER;
const byte PageManager::ARCH_PAGE_PRESENT;
const byte PageManager::ARCH_PAGE_RW;
const byte PageManager::ARCH_PAGE_READ_ONLY;
const byte PageManager::ARCH_PAGE_USER;
const byte PageManager::ARCH_PAGE_KERNEL;
const int  PageManager::ARCH_PAGE_SIZE;
const int  PageManager::ARCH_PAGE_TABLE_NUM;

PageManager::PageManager(dword totalMemorySize) {

    dword pageNumber = totalMemorySize / ARCH_PAGE_SIZE + ((totalMemorySize % ARCH_PAGE_SIZE) ? 1 : 0);

    memoryMap_ = new BitMap(pageNumber);
    if (memoryMap_ == NULL) panic("PageManager initilize error\n");
}

bool PageManager::allocatePhysicalPage(PageEntry* pageEntry) {

    int foundMemory = memoryMap_->find();

    /* no free memory found */
    if (foundMemory == -1) return false;

    /* set physical page */
    (*pageEntry) &= 0xFFF;
    (*pageEntry) |= (foundMemory * 4096);
    (*pageEntry) |= ARCH_PAGE_PRESENT;

    return true;
}

void PageManager::setup() {

    PageEntry* table = allocatePageTable();
    g_page_directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        memoryMap_->mark(i);
        makePageEntry(&(table[i]), 4096 * i, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);
    }

    memset(g_page_directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(g_page_directory, (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);

    setPageDirectory((PhysicalAddress)g_page_directory);
    startPaging();
}

PageEntry* PageManager::createNewPageDirectory() {

    PageEntry* table     = allocatePageTable();
    PageEntry* directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        makePageEntry(&(table[i]), 4096 * i, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_KERNEL);
    }

    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(directory, (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_KERNEL);

    dword directoryIndex = 0xFFFFFC00 >> 22;
    dword tableIndex     = (0xFFFFFC00 >> 12) & 0x3FF;

    /* test code. stack is always 0xFFFFFFFF */
    table = allocatePageTable();
    memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(&(directory[directoryIndex]), (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);
    allocatePhysicalPage(&(table[tableIndex]));

    return directory;
}

void PageManager::setPageDirectory(PhysicalAddress address) {

    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

void PageManager::startPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::stopPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x7fffffff, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::flushPageCache() const {

    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::makePageEntry(PageEntry* entry, PhysicalAddress address, byte present, byte rw, byte user) const {

    *entry = present | rw | user | (address & 0xfffff000);
}

PageEntry* PageManager::allocatePageTable() const {

    PageEntry* table;

    table = (PageEntry*)malloc(sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM * 2);

    if (table == NULL) panic("Page Table memory allocate error\n");
    for (; (dword)table % 4096; table++);

    return table;
}

bool PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress address, byte rw, byte user) {

    /* if already allocated, change attribute */

    return true;
}

bool PageManager::pageFaultHandler(LinearAddress address, dword error) {

    PageEntry* table;
    dword directoryIndex = address >> 22;
    dword tableIndex     = (address >> 12) & 0x3FF;
    byte  user           = address >= 0x4000000 ? ARCH_PAGE_USER : ARCH_PAGE_KERNEL;

    /* physical page not exist */
    if (error & 0x01 == ARCH_FAULT_NOT_EXIST) {

        if (isPresent(&(g_page_directory[directoryIndex]))) {

            table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            makePageEntry(&(g_page_directory[directoryIndex]), (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, user);
        }

        bool allocateResult = allocatePhysicalPage(&(table[tableIndex]));
        if (allocateResult) flushPageCache();

        return allocateResult;

    /* access falut */
    } else {

        g_console->printf("access denied.Process %s killed", g_current_process->name);
        g_process_manager->kill(g_current_process);
        return true;
    }

}

StackSegment::StackSegment(LinearAddress start, dword size) {

    start_        = start;
    size_         = size + PageManager::ARCH_PAGE_SIZE;
    isAutoExtend_ = false;

    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, start, PageManager::ARCH_PAGE_RW, g_current_process->dpl);
}

StackSegment::StackSegment(LinearAddress start, dword initileSize, dword maxSize) {

    start_        = start;
    size_         = initileSize + PageManager::ARCH_PAGE_SIZE;
    maxSize_      = maxSize;
    isAutoExtend_ = true;

    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, start, PageManager::ARCH_PAGE_RW, g_current_process->dpl);
}

StackSegment::~StackSegment() {

}

bool StackSegment::faultHandler(LinearAddress address, dword error) {

    if (error == PageManager::FAULT_NOT_WRITABLE && !tryExtend(address)) {

        return false;

    } else if (error == PageManager::FAULT_NOT_EXIST && !allocatePage(address)) {

        return false;
    } else {

        return false;
    }

    return true;
}

bool StackSegment::tryExtend(LinearAddress address) {

    if (!isAutoExtend_) {

        /* not auto extension mode */
        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    if (address > start_ + PageManager::ARCH_PAGE_SIZE || address < start_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    if (size_ + PageManager::ARCH_PAGE_SIZE > maxSize_) {

        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    /* page allocation */
    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3
                                 , address, PageManager::ARCH_PAGE_RW, g_current_process->dpl);
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3
                                         , address - PageManager::ARCH_PAGE_SIZE, PageManager::ARCH_PAGE_READ_ONLY, g_current_process->dpl);

    /* extention done */
    size_ += PageManager::ARCH_PAGE_SIZE;

    return true;
}

bool StackSegment::allocatePage(LinearAddress address) {

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, PageManager::ARCH_PAGE_RW, g_current_process->dpl);

    return true;
}

HeapSegment::HeapSegment(LinearAddress start, dword size) {

    start_ = start;
    size_  = size;
}

HeapSegment::~HeapSegment() {

}

bool HeapSegment::faultHandler(LinearAddress address, dword error) {

    if (error != PageManager::FAULT_NOT_EXIST) {

        errorNumber_ = FAULT_UNKNOWN;
        return false;
    }

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, PageManager::ARCH_PAGE_RW, g_current_process->dpl);


    return true;
}
