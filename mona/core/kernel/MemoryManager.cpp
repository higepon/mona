/*!
    \file   MemoryManger.cpp
    \brief  MemoryMangement allocate and free

    Copyright (c) 2002,2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    Al rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/28 update:$Date$
*/

#include "MemoryManager.h"
#include "global.h"

#define MM_MAGIC  0x46495963
#define HEADER_SIZE sizeof(Header)

MemoryManager::MemoryManager()
{
}

void MemoryManager::initialize(dword start, dword end)
{
    freeList = (Header*)start;
    freeList->next = freeList;
    freeList->size = (end - start + 1) / HEADER_SIZE;
    freeList->magic = MM_MAGIC;
    this->start = start;
    this->end   = end;
}

MemoryManager::~MemoryManager()
{
}

void MemoryManager::setNext(Header* p, Header* next)
{
    p->next = next;
}

void* MemoryManager::allocate(dword size)
{
    if (size == 0) return (dword)NULL;

    size = (size + 16 - 1) & 0xFFFFFFF0; /* align 16 */

    dword nunits = (size + HEADER_SIZE - 1) / HEADER_SIZE + 1;
    Header* prevp = freeList;
    Header *p;
    for (p = prevp->next; ;prevp = p, p = p->next)
    {
        if (p->size >= nunits) break;
        if (p == freeList) return 0;
    }

    if (p->size == nunits)
    {
	setNext(prevp, p->next);
    }
    else
    {
        Header* next = p + nunits;
	setNext(next, (p == p->next) ? next : p->next);
        next->size = p->size - nunits;
        p->size = nunits;
	setNext(prevp, next);
        if (p == freeList)
        {
            freeList = next;
        }
    }
    p->magic = MM_MAGIC;
    memset((void*)(p + 1), 0, size);

    return (void *)(p + 1);
}

void MemoryManager::debugprint()
{
    for (Header* p = freeList->next; ; p = p->next)
    {
        if (p->next == freeList) break;
    }

}

void MemoryManager::free(void* address)
{
    Header *bp = (Header *)address - 1;
    Header* a;
    Header* b;
    Header* p;
    bool inBetween = false;
    Header* tail = NULL;
    Header* prev;

    if (bp->magic != MM_MAGIC) {
	g_console->printf("kernel memory leaked?");
    }

    for (p = freeList, prev = freeList; ;prev = p, p = p->next)
    {
	if (p <= bp && bp <= p->next)
	{
	    inBetween = true;
	    break;
	}
	if (p->next == freeList)
	{
	    tail = p;
	    break;
	}
    }


    if (bp < freeList)
    {
	a = tail;
	b = freeList;
	tail->next = bp;
	freeList = bp;
    }
    else if (inBetween)
    {
	a = p;
	b = p->next;
    }
    else
    {
	a = tail;
	b = freeList;
    }

    if (a + a->size == bp && bp + bp->size == b)
    {
	a->size += bp->size + b->size;
	a->next = b->next;
    }
    else if (a + a->size == bp)
    {
	a->size += bp->size;
    }
    else if (bp + bp->size == b)
    {
	bp->next = b->next;
	a->next = bp;
	bp->size += b->size;
    }
    else
    {
	a->next = bp;
	bp->next = b;
    }

    return;
}

dword MemoryManager::getPhysicalMemorySize() {

    /* assume there is at least 1MB memory */
    dword totalMemorySize  = 1024 * 1024;

    /* 1MB unit loop */
    for (dword i = 1024 * 1024; i < 0xFFFFFFFF; i += 1024 * 1024) {

        volatile dword* p = (dword*)i;
        dword value = *p;

        *p = 0x12345678;
        if (*p != 0x12345678) break;

        *p = value;
        totalMemorySize += 1024 * 1024;
    }

    return totalMemorySize;
}

dword MemoryManager::getFreeMemorySize() const {
    dword result = 0;
    for (Header* current = freeList; current->next != freeList; current = current->next) {

        result += (current->size);
    }
    return result * HEADER_SIZE;
}

dword MemoryManager::getUsedMemorySize() const {
   return (end - start + 1) / HEADER_SIZE - getFreeMemorySize();
}

