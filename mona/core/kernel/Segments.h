/*!
    \file  Segments.h
    \brief Segment memory management

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
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
    SharedMemoryObject(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress);
    void initilize(uint32_t id, uint32_t size);
    virtual ~SharedMemoryObject();

    inline virtual int getAttachedCount() const {
        return attachedCount_;
    }

    inline virtual void setAttachedCount(int count) {
        attachedCount_ = count;
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
    static bool open(uint32_t id, uint32_t size, uint32_t pid, uint32_t linearAddress);
    static bool attach(uint32_t id, struct Process* process, LinearAddress address);
    static bool detach(uint32_t id, struct Process* process);
    static SharedMemoryObject* find(uint32_t id);
    enum
    {
        UN_MAPPED = -1,
        FLAG_NOT_SHARED = 0x01,
    };

  private:
    uint32_t id_;
    uint32_t size_;
    int attachedCount_;
    int physicalPageCount_;
    int* physicalPages_;
    uint32_t* flags_;
};

class Segment {

  public:
    Segment() {}
    virtual ~Segment() {}
    virtual bool faultHandler(LinearAddress address, uint32_t error) = 0;

    inline virtual int getErrorNumber() {
        return errorNumber_;
    }

    inline virtual LinearAddress getStart() {
        return start_;
    }

    inline virtual uint32_t getSize() {
        return size_;
    }

    inline virtual bool inRange(LinearAddress address) {

        return (address >= start_ && address < start_ + size_);
    }

  protected:
    LinearAddress start_;
    uint32_t         size_;
    uint8_t errorNumber_;

  public:
    static const uint8_t FAULT_STACK_OVERFLOW = 0x01;
    static const uint8_t FAULT_OUT_OF_RANGE   = 0x02;
    static const uint8_t FAULT_UNKNOWN        = 0x03;
};

class StackSegment : public Segment {

  public:
    StackSegment(LinearAddress start, uint32_t size);
    virtual ~StackSegment();
    virtual bool inRange(LinearAddress address) {
        return (start_ - size_ <= address && address <= start_);
    }

  public:
    virtual bool faultHandler(LinearAddress address, uint32_t error);
};

class HeapSegment : public Segment {

  public:
    HeapSegment(LinearAddress start, uint32_t size);
    virtual ~HeapSegment();

  public:
    virtual bool faultHandler(LinearAddress address, uint32_t error);
};

class SharedMemorySegment : public Segment {

  public:
    SharedMemorySegment();
    SharedMemorySegment(LinearAddress start, uint32_t size, SharedMemoryObject* sharedMemoryObject, bool writable = true);
    virtual ~SharedMemorySegment();

  public:
    virtual bool faultHandler(LinearAddress address, uint32_t error);
    inline virtual uint32_t getId() const {
        return sharedMemoryObject_->getId();
    }

    inline virtual void setWritable(bool writable) {
        writable_ = writable;
    }

  public:
    static SharedMemorySegment* find(Process* process, uint32_t id);

  protected:
    SharedMemoryObject* sharedMemoryObject_;
    bool writable_;
};

#endif
