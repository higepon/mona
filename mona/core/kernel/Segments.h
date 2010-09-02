/*!
    \file  Segments.h
    \brief Segment memory management

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/10/19 update:$Date$
*/
#ifndef _MONA_SEGMENTS_
#define _MONA_SEGMENTS_

#include "PageManager.h"
#include "operator.h"
#include "kernel.h"
#include "string.h"
#include "Process.h"

class SharedMemoryObject {

  public:
    SharedMemoryObject();
    SharedMemoryObject(uint32_t id, uint32_t size);
//    SharedMemoryObject(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress);
    void initilize(uint32_t id, uint32_t size);
    virtual ~SharedMemoryObject();

    void addRef() {
        refCount_++;
    }

    bool releaseRef() {
        refCount_--;
        ASSERT(refCount_ >= 0);
        return refCount_ == 0;
    }


    inline virtual uint32_t getId() const {
        return id_;
    }

    inline virtual uint32_t getSize() const {
        return size_;
    }

    inline virtual int isMapped(int physicalIndex) const {

        if (physicalIndex >= physicalPageCount_) return UN_MAPPED;
        return physicalPages_[physicalIndex];
    }

    inline virtual void map(int physicalIndex, PhysicalAddress address) {

        if (physicalIndex >= physicalPageCount_) return;

        physicalPages_[physicalIndex] = address;
    }

    inline virtual uint32_t getPageFlag(int physicalIndex) const {

        if (physicalIndex >= physicalPageCount_) return 0;
        return flags_[physicalIndex];
    }

    inline virtual void setPageFlag(int physicalIndex, uint32_t flag) {

        if (physicalIndex >= physicalPageCount_) return;

        flags_[physicalIndex] = flag;
    }

  public:
    static void setup();
    static bool open(uint32_t id, uint32_t size);
//    static bool open(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress);
    intptr_t attach(PageManager* pageManager, Process* process, LinearAddress address, bool isImmediateMap);
    intptr_t detach(PageManager* pageManager, Process* process);
    static SharedMemoryObject* find(uint32_t id);
    enum
    {
        UN_MAPPED = -1,
        FLAG_NOT_SHARED = 0x01,
    };

  private:
    uint32_t id_;
    uint32_t size_;
    int refCount_;
    int physicalPageCount_;
    int* physicalPages_;
    uint32_t* flags_;
};

class Segment
{
public:
    Segment(LinearAddress start, uint32_t size) : start_(start), size_(size)
    {
    }

    virtual ~Segment()
    {
    }

    virtual bool inRange(LinearAddress address)
    {
        return (address >= start_ && address < start_ + size_);
    }

    LinearAddress getStart() const
    {
        return start_;
    }

    uint32_t getSize()
    {
        return size_;
    }

    virtual bool faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
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

private:
    LinearAddress start_;
    uint32_t size_;
};

class SharedMemorySegment : public Segment
{
  public:
    SharedMemorySegment(LinearAddress start, uint32_t size, SharedMemoryObject* sharedMemoryObject, bool writable = true) :
        Segment(start, size),
        sharedMemoryObject_(sharedMemoryObject),
        writable_(writable)
    {
    }

    virtual ~SharedMemorySegment()
    {
    }

    virtual bool faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error)
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

    uint32_t getId() const
    {
        return sharedMemoryObject_->getId();
    }

    void setWritable(bool writable)
    {
        writable_ = writable;
    }

    SharedMemoryObject* getSharedMemoryObject() const
    {
        return sharedMemoryObject_;
    }


private:
    SharedMemoryObject* sharedMemoryObject_;
    bool writable_;
};

#endif
