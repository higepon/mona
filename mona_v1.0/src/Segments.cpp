/*!
  \file  Segments.cpp
  \brief class Segment memory management

  Copyright (c) 2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/10/19 update:$Date$
*/

#include <global.h>
#include <Segments.h>

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
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::StackSegment(LinearAddress start, dword initileSize, dword maxSize) {

    start_        = start;
    size_         = initileSize + PageManager::ARCH_PAGE_SIZE;
    maxSize_      = maxSize;
    isAutoExtend_ = true;

    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, start, true, false, true);
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
    \date   create:2003/10/15 update:2003/10/19
*/
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

/*!
    \brief try to extend stack, when auto extention mode

    \param  address fault point
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::tryExtend(LinearAddress address) {

    g_console->printf("extend start");

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
    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, address, true, true, true);

    /* read only */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3
                                         , address - PageManager::ARCH_PAGE_SIZE, true, false, (bool)g_current_process->dpl);

    /* extention done */
    size_ += PageManager::ARCH_PAGE_SIZE;

    g_console->printf("extend OK");

    return true;
}

/*!
    \brief allocate physical page

    \param  address LinearAddress
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::allocatePage(LinearAddress address) {

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, true, true, (bool)(g_current_process->dpl));

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
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, true, true, (bool)(g_current_process->dpl));

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
SharedMemorySegment::SharedMemorySegment(LinearAddress start, dword size, SharedMemoryObject* sharedMemoryObject) {

    start_ = start;
    size_  = size;
    sharedMemoryObject_ = sharedMemoryObject;
}

/*!
    \brief construct segment

    \author HigePon
    \date   create:2003/10/28 update:
*/
SharedMemorySegment::SharedMemorySegment() {

    /* dummy */
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemorySegment::~SharedMemorySegment() {

}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/25 update:
*/
bool SharedMemorySegment::faultHandler(LinearAddress address, dword error) {

    int mapResult;

    //    g_console->printf("[shared] address = %x", address);

    if (error != PageManager::FAULT_NOT_EXIST) {

        errorNumber_ = FAULT_UNKNOWN;
        return false;
    }

    if (address < start_ || address > start_ + size_) {

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

    if (mappedAddress == SharedMemoryObject::UN_MAPPED) {

        mapResult = g_page_manager->allocatePhysicalPage((PageEntry*)(g_current_process->cr3),address, true, true, true);

        sharedMemoryObject_->map(physicalIndex, mapResult == -1 ? SharedMemoryObject::UN_MAPPED : mapResult);

    } else {

        mapResult = g_page_manager->allocatePhysicalPage((PageEntry*)(g_current_process->cr3), address, mappedAddress, true, true, true);
    }

    return (mapResult != -1);
}

/*!
    \brief find SharedMemorySegment that has the ID

    \param  id ID for SharedMemorySegment
    \author HigePon
    \date   create:2003/10/29 update:
*/
SharedMemorySegment* SharedMemorySegment::find(SharedMemorySegment* head, dword id) {

    SharedMemorySegment* current;
    SharedMemorySegment* result = NULL;

    for (current = head; ; current = (SharedMemorySegment*)current->getNext()) {

        /* found */
        if (id == current->getId()) {

            result = current;
            break;
        }

        if ((SharedMemorySegment*)current->getNext() == head) break;
    }
    return result;
}

/*----------------------------------------------------------------------
    SharedMemoryObject
----------------------------------------------------------------------*/

/*!
    \brief initilize SharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemoryObject::SharedMemoryObject() {

    /* dummy for g_sharedMemoryList */
}

/*!
    \brief initilize SharedMemoryObject

    \param id   shared memory object ID of identify
    \param size shared memory size

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemoryObject::SharedMemoryObject(dword id, dword size) {

    if (size == 0) return;

    /* check dup id */


    physicalPageCount_ = size / 4096;
    physicalPages_     = new int[physicalPageCount_];

    if (physicalPages_ == NULL) panic("SharedMemoryObject: new failed");
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
SharedMemoryObject::~SharedMemoryObject() {

    for (int i = 0; i < physicalPageCount_; i++) {

        g_page_manager->returnPhysicalPage(physicalPages_[i]);
    }

    delete(physicalPages_);

    return;
}

/*!
    \brief set up karnel for using sharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:
*/
void SharedMemoryObject::setup() {

    g_sharedMemoryList = new SharedMemoryObject();
    QueueManager::init(g_sharedMemoryList);
}

/*!
    \brief find sharedMemoryObject that has the ID

    \param id id for sharedMemoryObject

    \author HigePon
    \date   create:2003/10/25 update:
*/
SharedMemoryObject* SharedMemoryObject::find(dword id) {

    SharedMemoryObject* current;
    SharedMemoryObject* result = NULL;

    for (current = g_sharedMemoryList; ; current = (SharedMemoryObject*)current->getNext()) {

        /* found */
        if (id == current->getId()) {

            result = current;
            break;
        }

        if ((SharedMemoryObject*)current->getNext() == g_sharedMemoryList) break;
    }

    return result;
}

/*!
    \brief open sharedMemoryObject

    if sharedMemoryObject id not present. create one.

    \param id   id for sharedMemoryObject
    \param size size of sharedMemory
    \author HigePon
    \date   create:2003/10/25 update:
*/
bool SharedMemoryObject::open(dword id, dword size) {

    SharedMemoryObject* target = find(id);

    /* new SharedMemory */
    if (target == NULL) {

        target = new SharedMemoryObject(id, size);
        if (target == NULL) panic("SharedMemory open: failed");

        QueueManager::addToPrevious(g_sharedMemoryList, target);

    } else {

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
    \date   create:2003/10/25 update:
*/
bool SharedMemoryObject::attach(dword id, struct ProcessInfo* process, LinearAddress address) {

    SharedMemorySegment* segment;
    SharedMemoryObject* target = find(id);
    if (target == NULL) return false;

    segment = new SharedMemorySegment(address, target->getSize(), target);
    if (segment == NULL) return false;

    QueueManager::addToPrevious(process->shared, segment);
    target->setAttachedCount(target->getAttachedCount() + 1);

    return true;
}

/*!
    \brief detach sharedMemoryObject from process

    \param id      id for sharedMemoryObject
    \param process process

    \author HigePon
    \date   create:2003/10/25 update:2003/10/29
*/
bool SharedMemoryObject::detach(dword id, struct ProcessInfo* process) {

    SharedMemoryObject* target = find(id);
    if (target == NULL) return false;

    SharedMemorySegment* segment = SharedMemorySegment::find(process->shared, id);
    if (segment == NULL) return false;

    g_page_manager->setAbsent((PageEntry*)process->cr3, process->shared->getStart(), process->shared->getSize());

    QueueManager::remove(segment);
    delete(segment);

    target->setAttachedCount(target->getAttachedCount() - 1);

    /* should be removed */
    if (target->getAttachedCount() == 0) {

        QueueManager::remove(target);
        delete(target);
    }

    return true;
}
