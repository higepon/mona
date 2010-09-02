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


bool Segment::faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
{
    ASSERT(inRange(address));

    if (error != PageManager::FAULT_NOT_EXIST) {
        return false;
    }

    pageManager->mapOnePage(process->getPageDirectory(),
                            address,
                            PageManager::PAGE_WRITABLE,
                            process->isUserMode());
    return true;
}

bool SharedMemorySegment::faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
{
    ASSERT(inRange(address));

    if (error != PageManager::FAULT_NOT_EXIST) {
        return false;
    }

    uint32_t faultTableIndex = PageManager::getTableIndex(address);
    uint32_t faultDirectoryIndex = PageManager::getDirectoryIndex(address);

    uint32_t startTableIndex = PageManager::getTableIndex(getStart());
    uint32_t startDirectoryIndex = PageManager::getDirectoryIndex(getStart());

    uint32_t physicalIndex = faultTableIndex + faultDirectoryIndex * 1024 - startTableIndex - startDirectoryIndex * 1024;

    int mappedAddress   = sharedMemoryObject_->isMapped(physicalIndex);
    uint32_t pageFlag = sharedMemoryObject_->getPageFlag(physicalIndex);

    int mapResult;
    if (pageFlag & SharedMemoryObject::FLAG_NOT_SHARED) {
        mapResult = pageManager->mapOnePage(process->getPageDirectory(),
                                            address,
                                            PageManager::PAGE_WRITABLE,
                                            PageManager::PAGE_USER);
    } else if (mappedAddress == SharedMemoryObject::UN_MAPPED) {
        mapResult = pageManager->mapOnePage(process->getPageDirectory(), address, writable_, PageManager::PAGE_USER);
        sharedMemoryObject_->map(physicalIndex, mapResult == -1 ? SharedMemoryObject::UN_MAPPED : mapResult);
    } else {
        mapResult = pageManager->mapOnePageByPhysicalAddress(process->getPageDirectory(),
                                                             address,
                                                             mappedAddress,
                                                             writable_,
                                                             PageManager::PAGE_USER);
    }
    return (mapResult != -1);
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

// SharedMemoryObject::SharedMemoryObject(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress)
// {
//     initilize(id, size);

//     Process* process = g_scheduler->FindProcess(pid);
//     if (process == NULL)
//     {
//         return;
//     }

//     PageEntry* table;
//     PageEntry* directory = process->getPageDirectory();

//     for (int i = 0; i < physicalPageCount_; i++, linearAddress += 4096)
//     {
//         uint32_t tableIndex     = PageManager::getTableIndex(linearAddress);
//         uint32_t directoryIndex = PageManager::getDirectoryIndex(linearAddress);

//         if (PageManager::isPresent(directory[directoryIndex]))
//         {
//             table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
//         } else
//         {
//             break;
//         }
//         physicalPages_[i] = table[tableIndex] & 0xFFFFF000;
//     }
// }

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
        if (physicalPages_[i] != UN_MAPPED) {
            g_page_manager->returnPhysicalPage(physicalPages_[i]);
        }
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
//    g_sharedMemoryObjectList = new HList<SharedMemoryObject*>();
      SharedMemoryObject::open(0x7000, 256 * 1024 * 1024);
    g_dllSharedObject = SharedMemoryObject::find(0x7000);
}

/*!
    \brief find sharedMemoryObject that has the ID

    \param id id for sharedMemoryObject

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
SharedMemoryObject* SharedMemoryObject::find(uint32_t id)
{
    return g_page_manager->findSharedMemoryObject(id);
    // SharedMemoryObject* current;
    // for (int i = 0; i < g_sharedMemoryObjectList->size(); i++)
    // {
    //     current = g_sharedMemoryObjectList->get(i);

    //     /* found */
    //     if (id == current->getId())
    //     {
    //         return current;
    //     }
    // }
    // return (SharedMemoryObject*)NULL;
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
    // SharedMemoryObject* target = find(id);
    // if (0 == size) return false;
    // /* new SharedMemory */
    // if (target == NULL)
    // {
    //     target = new SharedMemoryObject(id, size);
    //     checkMemoryAllocate(target, "SharedMemoryObject memory allocate target");
    //     g_sharedMemoryObjectList->add(target);

    // } else
    // {
    //     if (target->getSize() != size) return false;
    // }

    // return true;
    return g_page_manager->findOrCreateSharedMemoryObject(id, size) != NULL;
}

// bool SharedMemoryObject::open(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress)
// {
//     panic("hige");
//     SharedMemoryObject* target = find(id);

//     /* new SharedMemory */
//     if (target == NULL)
//     {
//         target = new SharedMemoryObject(id, size, pid, linearAddress);
//         checkMemoryAllocate(target, "SharedMemoryObject memory allocate target");
//         g_sharedMemoryObjectList->add(target);
//     } else
//     {
//         if (target->getSize() != size) return false;
//     }

//     return true;
// }

/*!
    \brief attach sharedMemory to process space

    \param id      id for sharedMemoryObject
    \param process target process
    \param address attach point at process space

    \author Higepon
    \date   create:2003/10/25 update:2004/01/08
*/
intptr_t SharedMemoryObject::attach(PageManager* pageManager, Process* process, LinearAddress address, bool isImmediateMap)
{
    uintptr_t start = address;
    uintptr_t end = start + getSize();

    if (process->hasSharedOverlap(start, end)) {
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
