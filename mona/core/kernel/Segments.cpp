/*!
  \file  Segments.cpp
  \brief class Segment memory management

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2003/10/19 update:$Date$
*/

#include <sys/HList.h>
#include "global.h"
#include "Segments.h"

/* Segment Faults */
const uint8_t Segment::FAULT_STACK_OVERFLOW;
const uint8_t Segment::FAULT_OUT_OF_RANGE;
const uint8_t Segment::FAULT_UNKNOWN;

/*----------------------------------------------------------------------
    StackSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize heap segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::StackSegment(LinearAddress start, uint32_t size) {

    start_ = start;
    size_  = size;
}

/*!
    \brief destruct segment

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::~StackSegment() {
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::faultHandler(LinearAddress address, uint32_t error) {

    if (error != PageManager::FAULT_NOT_EXIST) {

        errorNumber_ = FAULT_UNKNOWN;
        return false;
    }

    if (start_ - size_ > address || start_ < address) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    Process* current = g_currentThread->process;
    g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, true, true, current->isUserMode());

    return true;
}


/*----------------------------------------------------------------------
    HeapSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize heap segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::HeapSegment(LinearAddress start, uint32_t size) {

    start_ = start;
    size_  = size;
}

/*!
    \brief destruct segment

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::~HeapSegment() {
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
bool HeapSegment::faultHandler(LinearAddress address, uint32_t error) {

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
    \author Higepon
    \date   create:2003/10/25 update:
*/
SharedMemorySegment::SharedMemorySegment(LinearAddress start, uint32_t size, SharedMemoryObject* sharedMemoryObject, bool writable /* = true */)
{
    start_ = start;
    size_  = size;
    writable_ = writable;
    sharedMemoryObject_ = sharedMemoryObject;
}

/*!
    \brief construct segment

    \author Higepon
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

    \author Higepon
    \date   create:2003/10/25 update:
*/
SharedMemorySegment::~SharedMemorySegment()
{
}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author Higepon
    \date   create:2003/10/25 update:
*/
bool SharedMemorySegment::faultHandler(LinearAddress address, uint32_t error)
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
    uint32_t tableIndex1     = PageManager::getTableIndex(address);
    uint32_t directoryIndex1 = PageManager::getDirectoryIndex(address);

    /* segment start point */
    uint32_t tableIndex2     = PageManager::getTableIndex(start_);
    uint32_t directoryIndex2 = PageManager::getDirectoryIndex(start_);

    /* check already allocated physical page? */
    uint32_t physicalIndex = tableIndex1 + directoryIndex1 * 1024 - tableIndex2 - directoryIndex2 * 1024;

    int mappedAddress   = sharedMemoryObject_->isMapped(physicalIndex);
    uint32_t pageFlag = sharedMemoryObject_->getPageFlag(physicalIndex);
    Process* current = g_currentThread->process;

    if (pageFlag & SharedMemoryObject::FLAG_NOT_SHARED)
    {
        mapResult = g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, true, true, true);
    }
    else if (mappedAddress == SharedMemoryObject::UN_MAPPED)
    {
        mapResult = g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, true, writable_, true);
        sharedMemoryObject_->map(physicalIndex, mapResult == -1 ? SharedMemoryObject::UN_MAPPED : mapResult);
    } else
    {
        mapResult = g_page_manager->allocatePhysicalPage(current->getPageDirectory(), address, mappedAddress, true, writable_, true);
    }
    return (mapResult != -1);
}

/*!
    \brief find SharedMemorySegment that has the ID

    \param  id ID for SharedMemorySegment
    \author Higepon
    \date   create:2003/10/29 update:2004/01/08
*/
SharedMemorySegment* SharedMemorySegment::find(Process* process, uint32_t id)
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

    \author Higepon
    \date   create:2003/10/25 update:2003/01/08
*/
SharedMemoryObject::SharedMemoryObject(uint32_t id, uint32_t size)
{
    initilize(id, size);
    return;
}

SharedMemoryObject::SharedMemoryObject(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress)
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
        uint32_t tableIndex     = PageManager::getTableIndex(linearAddress);
        uint32_t directoryIndex = PageManager::getDirectoryIndex(linearAddress);

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

void SharedMemoryObject::initilize(uint32_t id, uint32_t size)
{
    if (size <= 0) return;

    physicalPageCount_ = size / 4096;
    physicalPages_     = new int[physicalPageCount_];
    checkMemoryAllocate(physicalPages_, "SharedMemoryObject memory allocate physicalPages");
    memset(physicalPages_, UN_MAPPED, sizeof(int) * physicalPageCount_);

    flags_ = new uint32_t[physicalPageCount_];
    checkMemoryAllocate(flags_, "SharedMemoryObject memory allocate flags");
    memset(flags_, 0, sizeof(uint32_t) * physicalPageCount_);

    size_ = size;
    id_   = id;
    return;
}

/*!
    \brief destruct SharedMemoryObject

    \author Higepon
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

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
void SharedMemoryObject::setup()
{
    g_sharedMemoryObjectList = new HList<SharedMemoryObject*>();

    SharedMemoryObject::open(0x7000, 256 * 1024 * 1024);
    g_dllSharedObject = SharedMemoryObject::find(0x7000);
;}

/*!
    \brief find sharedMemoryObject that has the ID

    \param id id for sharedMemoryObject

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
SharedMemoryObject* SharedMemoryObject::find(uint32_t id)
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
    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::open(uint32_t id, uint32_t size)
{
    SharedMemoryObject* target = find(id);

    if (0 == size) return false;

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

bool SharedMemoryObject::open(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress)
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

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::attach(uint32_t id, Process* process, LinearAddress address)
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

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
bool SharedMemoryObject::detach(uint32_t id, Process* process)
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
//        logprintf("segment id = %x removed %s(%s):%d\n", id,__FILE__, __func__, __LINE__);
//        logprintf("map removed id=%x %s:%d:(%s)\n", id, __FILE__, __LINE__, __func__);
        g_sharedMemoryObjectList->remove(target);
        delete(target);
    }
    return true;
}
