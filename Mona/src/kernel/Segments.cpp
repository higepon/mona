/*!
  \file  Segments.cpp
  \brief class Segment memory management

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/10/19 update:$Date$
*/

#include <sys/HList.h>
#include "global.h"
#include "Segments.h"

/* Segment Faults */
const byte Segment::FAULT_STACK_OVERFLOW;
const byte Segment::FAULT_OUT_OF_RANGE;
const byte Segment::FAULT_UNKNOWN;

/*----------------------------------------------------------------------
    StackSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize stack segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::StackSegment(LinearAddress start, dword size) {

    start_        = start;
    size_         = size + PageManager::ARCH_PAGE_SIZE;
    isAutoExtend_ = false;
}

/*!
    \brief initilize stack segment. Auto extention mode

    \param  start        LinearAddress of start
    \param  initilieSize segment size
    \param  maxSize      segment max size
    \author HigePon
    \date   create:2003/10/15 update:2004/01/08
*/
StackSegment::StackSegment(Process* process, LinearAddress start, dword initileSize, dword maxSize) {

    start_        = start;
    size_         = initileSize + PageManager::ARCH_PAGE_SIZE;
    maxSize_      = maxSize;
    isAutoExtend_ = true;
    g_page_manager->setAttribute(process->getPageDirectory(), start, true, false, true);
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::~StackSegment() {
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/15 update:2003/11/06
*/
bool StackSegment::faultHandler(LinearAddress address, dword error) {

    g_console->printf("auto?");

    switch(error) {

    case PageManager::FAULT_NOT_WRITABLE:

        if (!tryExtend(g_currentThread->process, address)) {
            return false;
        }
        break;

    case PageManager::FAULT_NOT_EXIST:

        if (!allocatePage(g_currentThread->process, address)) {
            return false;
        }
        break;
    default:
        return false;
    }

    return true;
}

/*!
    \brief try to extend stack, when auto extention mode

    \param  address fault point
    \author HigePon
    \date   create:2003/10/15 update:2004/01/08
*/
bool StackSegment::tryExtend(Process* process, LinearAddress address) {

    g_console->printf("extend start");

    if (!isAutoExtend_) {

        /* not auto extension mode */
        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    if (size_ + PageManager::ARCH_PAGE_SIZE > maxSize_) {

        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    /* page allocation */
    if (!(g_page_manager->setAttribute(process->getPageDirectory(), address, true, true, true))) {

         panic("wan");
    }

    /* read only */
    g_page_manager->allocatePhysicalPage(process->getPageDirectory()
                                         , address - PageManager::ARCH_PAGE_SIZE, true, false, process->isUserMode());

    /* extention done */
    size_  += PageManager::ARCH_PAGE_SIZE;
    start_ -= PageManager::ARCH_PAGE_SIZE;

    g_console->printf("extend OK");

    return true;
}

/*!
    \brief allocate physical page

    \param  address LinearAddress
    \author HigePon
    \date   create:2003/10/15 update:2004/01/08
*/
bool StackSegment::allocatePage(Process* process, LinearAddress address) {

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage(process->getPageDirectory(), address, true, true, process->isUserMode());

    return true;
}

/*----------------------------------------------------------------------
    HeapSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize heap segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::HeapSegment(LinearAddress start, dword size) {

    start_ = start;
    size_  = size;
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::~HeapSegment() {
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
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
    Process* current = g_currentThread->process;
    g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, true, true, current->isUserMode());

    return true;
}

/*----------------------------------------------------------------------
    SharedMemorySegment
----------------------------------------------------------------------*/

/*!
    \brief initilize SharedMemorySegment

    \param  start              LinearAddress of start
    \param  size               segment size
    \param  sharedMemoryObject pointer to sharedMemoryObject
    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemorySegment::SharedMemorySegment(LinearAddress start, dword size, SharedMemoryObject* sharedMemoryObject)
{
    start_ = start;
    size_  = size;
    sharedMemoryObject_ = sharedMemoryObject;
}

/*!
    \brief construct segment

    \author HigePon
    \date   create:2003/10/28 update:
*/
SharedMemorySegment::SharedMemorySegment()
{
    /* dummy */
    start_ = 0;
    size_  = 0;
    sharedMemoryObject_ = 0;
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemorySegment::~SharedMemorySegment()
{
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/25 update:
*/
bool SharedMemorySegment::faultHandler(LinearAddress address, dword error)
{
    int mapResult;
    if (error != PageManager::FAULT_NOT_EXIST)
    {
        errorNumber_ = FAULT_UNKNOWN;
        return false;
    }

    if (address < start_ || address > start_ + size_)
    {
        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page fault point */
    dword tableIndex1     = PageManager::getTableIndex(address);
    dword directoryIndex1 = PageManager::getDirectoryIndex(address);

    /* segment start point */
    dword tableIndex2     = PageManager::getTableIndex(start_);
    dword directoryIndex2 = PageManager::getDirectoryIndex(start_);

    /* check already allocated physical page? */
    dword physicalIndex = tableIndex1 + directoryIndex1 * 1024 - tableIndex2 - directoryIndex2 * 1024;

    int mappedAddress   = sharedMemoryObject_->isMapped(physicalIndex);
    Process* current = g_currentThread->process;

    if (mappedAddress == SharedMemoryObject::UN_MAPPED)
    {
        mapResult = g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, true, true, true);
        sharedMemoryObject_->map(physicalIndex, mapResult == -1 ? SharedMemoryObject::UN_MAPPED : mapResult);
    } else
    {
        mapResult = g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, mappedAddress, true, true, true);
    }
    return (mapResult != -1);
}

/*!
    \brief find SharedMemorySegment that has the ID

    \param  id ID for SharedMemorySegment
    \author HigePon
    \date   create:2003/10/29 update:2004/01/08
*/
SharedMemorySegment* SharedMemorySegment::find(Process* process, dword id)
{
    List<SharedMemorySegment*>* list = process->getSharedList();

    for (int i = 0; i < list->size(); i++)
    {
        SharedMemorySegment* segment = list->get(i);

        /* found */
        if (id == segment->getId())
        {
            return segment;
        }
    }
    return (SharedMemorySegment*)NULL;
}

/*----------------------------------------------------------------------
    SharedMemoryObject
----------------------------------------------------------------------*/

/*!
    \brief initilize SharedMemoryObject

    \param id   shared memory object ID of identify
    \param size shared memory size

    \author HigePon
    \date   create:2003/10/25 update:2003/01/08
*/
SharedMemoryObject::SharedMemoryObject(dword id, dword size)
{
    initilize(id, size);
    return;
}

SharedMemoryObject::SharedMemoryObject(dword id, dword size, dword pid, dword linearAddress)
{
    initilize(id, size);

    Process* process = g_scheduler->FindProcess(pid);
    if (process == NULL)
    {
        return;
    }

    PageEntry* table;
    PageEntry* directory = process->getPageDirectory();

    for (int i = 0; i < physicalPageCount_; i++, linearAddress += 4096)
    {
        dword tableIndex     = PageManager::getTableIndex(linearAddress);
        dword directoryIndex = PageManager::getDirectoryIndex(linearAddress);

        if (PageManager::isPresent(&(directory[directoryIndex])))
        {
            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else
        {
            break;
        }
        physicalPages_[i] = table[tableIndex] & 0xFFFFF000;
    }
}

void SharedMemoryObject::initilize(dword id, dword size)
{
    if (size <= 0) return;

    physicalPageCount_ = size / 4096;
    physicalPages_     = new int[physicalPageCount_];
    checkMemoryAllocate(physicalPages_, "SharedMemoryObject memory allocate physicalPages");
    memset(physicalPages_, UN_MAPPED, sizeof(int) * physicalPageCount_);

    size_ = size;
    id_   = id;
    return;
}

/*!
    \brief destruct SharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemoryObject::~SharedMemoryObject()
{
    for (int i = 0; i < physicalPageCount_; i++) {

        g_page_manager->returnPhysicalPage(physicalPages_[i]);
    }

    delete[] physicalPages_;
    return;
}

/*!
    \brief set up karnel for using sharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:2004/01/08
*/
void SharedMemoryObject::setup()
{
    g_sharedMemoryObjectList = new HList<SharedMemoryObject*>();
}

/*!
    \brief find sharedMemoryObject that has the ID

    \param id id for sharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:2004/01/08
*/
SharedMemoryObject* SharedMemoryObject::find(dword id)
{
    SharedMemoryObject* current;

    for (int i = 0; i < g_sharedMemoryObjectList->size(); i++)
    {
        current = g_sharedMemoryObjectList->get(i);

        /* found */
        if (id == current->getId())
        {
            return current;
        }
    }
    return (SharedMemoryObject*)NULL;
}

/*!
    \brief open sharedMemoryObject

    if sharedMemoryObject id not present. create one.

    \param id   id for sharedMemoryObject
    \param size size of sharedMemory
    \author HigePon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::open(dword id, dword size)
{
    SharedMemoryObject* target = find(id);

    /* new SharedMemory */
    if (target == NULL)
    {
        target = new SharedMemoryObject(id, size);
        checkMemoryAllocate(target, "SharedMemoryObject memory allocate target");
        g_sharedMemoryObjectList->add(target);

    } else
    {
        if (target->getSize() != size) return false;
    }

    return true;
}

bool SharedMemoryObject::open(dword id, dword size, dword pid, dword linearAddress)
{
    SharedMemoryObject* target = find(id);

    /* new SharedMemory */
    if (target == NULL)
    {
        target = new SharedMemoryObject(id, size, pid, linearAddress);
        checkMemoryAllocate(target, "SharedMemoryObject memory allocate target");
        g_sharedMemoryObjectList->add(target);
    } else
    {
        if (target->getSize() != size) return false;
    }

    return true;
}

/*!
    \brief attach sharedMemory to process space

    \param id      id for sharedMemoryObject
    \param process target process
    \param address attach point at process space

    \author HigePon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::attach(dword id, Process* process, LinearAddress address)
{
    SharedMemorySegment* segment;
    SharedMemoryObject* target = find(id);
    if (target == NULL)
    {
        return false;
    }

    segment = new SharedMemorySegment(address, target->getSize(), target);
    if (segment == NULL) return false;

    process->getSharedList()->add(segment);
    target->setAttachedCount(target->getAttachedCount() + 1);
    return true;
}

/*!
    \brief detach sharedMemoryObject from process

    \param id      id for sharedMemoryObject
    \param process process

    \author HigePon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::detach(dword id, Process* process)
{
    SharedMemoryObject* target = find(id);
    if (target == NULL) return false;

    SharedMemorySegment* segment = SharedMemorySegment::find(process, id);
    if (segment == NULL) return false;

    /* destroy */
    g_page_manager->setAbsent(process->getPageDirectory(), segment->getStart(), segment->getSize());
    process->getSharedList()->remove(segment);
    delete(segment);

    target->setAttachedCount(target->getAttachedCount() - 1);

    /* should be removed */
    if (target->getAttachedCount() == 0)
    {
        g_sharedMemoryObjectList->remove(target);
        delete(target);
    }
    return true;
}
