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

class SharedMemoryObject {
private:
    SharedMemoryObject(uint32_t id, uint32_t size);

public:
    static SharedMemoryObject* create(uint32_t size);
    static SharedMemoryObject* find(uint32_t id);
    static void destroy(SharedMemoryObject* shm);

    virtual ~SharedMemoryObject();

    void addRef()
    {
        refCount_++;
    }

    bool releaseRef()
    {
        refCount_--;
        MONA_ASSERT(refCount_ >= 0);
        return refCount_ == 0;
    }

    uint32_t getId() const
    {
        return id_;
    }

    uint32_t getSize() const
    {
        return size_;
    }

    uint32_t getMappedPhysicalAddress(int physicalIndex) const
    {
        MONA_ASSERT(physicalIndex < physicalPageCount_);
        return physicalPages_[physicalIndex];
    }

    void map(int physicalIndex, PhysicalAddress address)
    {
        if (physicalIndex >= physicalPageCount_) return;
        physicalPages_[physicalIndex] = address;
    }

    intptr_t attach(PageManager* pageManager, Process* process, LinearAddress address, bool isImmediateMap);
    intptr_t detach(PageManager* pageManager, Process* process);
    enum
    {
        UN_MAPPED = 0x00,
    };

  private:
    uint32_t id_;
    uint32_t size_;
    int refCount_;
    int physicalPageCount_;
    uint32_t* physicalPages_;

    static HList<SharedMemoryObject*> sharedList_;
};

class Segment
{
public:
    Segment() : start_(0), size_(0) {}
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

    virtual bool faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error);

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

    virtual bool faultHandler(PageManager* pageManager, Process* process, LinearAddress address, uint32_t error);

    uint32_t getId() const
    {
        return sharedMemoryObject_->getId();
    }

    SharedMemoryObject* getSharedMemoryObject() const
    {
        return sharedMemoryObject_;
    }


private:
    SharedMemoryObject* sharedMemoryObject_;
    const bool writable_;
};

#endif
