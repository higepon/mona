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

#include "global.h"
#include <sys/HList.h>
#include "Segments.h"
#include "PageManager.h"

HList<SharedMemoryObject*> SharedMemoryObject::sharedList_;

bool Segment::faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
{
    ASSERT(inRange(address));
    ASSERT(error == PageManager::FAULT_NOT_EXIST);

    PhysicalAddress mappedResultAddress;
    intptr_t ret = pageManager->mapOnePage(process->getPageDirectory(),
                                           mappedResultAddress,
                                           address,
                                           PageManager::PAGE_WRITABLE,
                                           process->isUserMode());
    ASSERT(ret == M_OK);
    return true;
}

bool SharedMemorySegment::faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
{
    ASSERT(inRange(address));
    ASSERT(error == PageManager::FAULT_NOT_EXIST);

    uint32_t faultTableIndex = PageManager::getTableIndex(address);
    uint32_t faultDirectoryIndex = PageManager::getDirectoryIndex(address);

    uint32_t startTableIndex = PageManager::getTableIndex(getStart());
    uint32_t startDirectoryIndex = PageManager::getDirectoryIndex(getStart());

    uint32_t physicalIndex = faultTableIndex + faultDirectoryIndex * 1024 - startTableIndex - startDirectoryIndex * 1024;

    uint32_t mappedAddress = sharedMemoryObject_->getMappedPhysicalAddress(physicalIndex);

    if (mappedAddress == SharedMemoryObject::UN_MAPPED) {
        PhysicalAddress mappedResultAddress;
        intptr_t ret = pageManager->mapOnePage(process->getPageDirectory(), mappedResultAddress, address, writable_, PageManager::PAGE_USER);
        sharedMemoryObject_->map(physicalIndex, ret == M_OK ? mappedResultAddress : SharedMemoryObject::UN_MAPPED);
    } else {
        pageManager->mapOnePageByPhysicalAddress(process->getPageDirectory(),
                                                 address,
                                                 mappedAddress,
                                                 writable_,
                                                 PageManager::PAGE_USER);
    }
    return true;
}


/*----------------------------------------------------------------------
    SharedMemoryObject
----------------------------------------------------------------------*/
SharedMemoryObject::SharedMemoryObject(uint32_t id, uint32_t size) :
    id_(id),
    size_(size),
    refCount_(0),
    physicalPageCount_(size / PageManager::ARCH_PAGE_SIZE)
{
    ASSERT(size != 0);

    physicalPages_ = new uint32_t[physicalPageCount_];
    ASSERT(physicalPages_);
    memset(physicalPages_, UN_MAPPED, sizeof(uint32_t) * physicalPageCount_);
}

SharedMemoryObject::~SharedMemoryObject()
{
    for (int i = 0; i < physicalPageCount_; i++) {
        if (physicalPages_[i] != (uint32_t)UN_MAPPED) {
            g_page_manager->returnPhysicalPage(physicalPages_[i]);
        }
    }
    delete[] physicalPages_;
    return;
}

void SharedMemoryObject::destroy(SharedMemoryObject* shm)
{
    if (shm->releaseRef()) {
        sharedList_.remove(shm);
        delete shm;
    }

}

SharedMemoryObject* SharedMemoryObject::find(uint32_t id)
{
    for (int i = 0; i < sharedList_.size(); i++) {
        SharedMemoryObject* shm = sharedList_.get(i);
         if (id == shm->getId()) {
             return shm;
         }
    }
    return NULL;
}

SharedMemoryObject* SharedMemoryObject::create(uint32_t size)
{
    static uint32_t id = 1;
    if (size == 0) {
        return NULL;
    }

    SharedMemoryObject* shm = new SharedMemoryObject(id++, size);
    ASSERT(shm);
    sharedList_.add(shm);
    return shm;
}


intptr_t SharedMemoryObject::attach(PageManager* pageManager, Process* process, LinearAddress address, bool isImmediateMap)
{
    uintptr_t start = address;
    uintptr_t end = start + getSize();

    if (process->hasSharedOverlap(start, end)) {
        ASSERT(true); // Bug!
        return M_BAD_ADDRESS;
    } else {
        SharedMemorySegment* segment = new SharedMemorySegment(address, getSize(), this);
        if (segment == NULL) {
            return M_NO_MEMORY;
        }
        process->getSharedList()->add(segment);
        addRef();

        //  If we know all memory region will be accessed,
        //  we avoid using demand paging which causes poor performance.
        if (isImmediateMap) {
            for (LinearAddress start = address; start < end; start += PageManager::ARCH_PAGE_SIZE) {
                bool isSucceed = segment->faultHandler(pageManager, process, start, PageManager::FAULT_NOT_EXIST);
                ASSERT(isSucceed);
            }
        }
        return M_OK;
    }
}

intptr_t SharedMemoryObject::detach(PageManager* pageManager, Process* process)
{
    SharedMemorySegment* segment = process->findSharedSegment(getId());
    if (segment == NULL) {
        return M_BAD_MEMORY_MAP_ID;
    }

    pageManager->unmapRange(process->getPageDirectory(), segment->getStart(), segment->getStart() + segment->getSize(), false);
    process->getSharedList()->remove(segment);
    delete segment;
    return M_OK;
}

