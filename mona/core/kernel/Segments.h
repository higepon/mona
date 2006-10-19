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
    SharedMemoryObject(dword id, dword size);
    SharedMemoryObject(dword id, dword size, dword pid, dword linearAddress);
    void initilize(dword id, dword size);
    virtual ~SharedMemoryObject();

    inline virtual int getAttachedCount() const {
        return attachedCount_;
    }

    inline virtual void setAttachedCount(int count) {
        attachedCount_ = count;
    }

    inline virtual dword getId() const {
        return id_;
    }

    inline virtual dword getSize() const {
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

    inline virtual dword getPageFlag(int physicalIndex) const {

        if (physicalIndex >= physicalPageCount_) return 0;
        return flags_[physicalIndex];
    }

    inline virtual void setPageFlag(int physicalIndex, dword flag) {

        if (physicalIndex >= physicalPageCount_) return;

        flags_[physicalIndex] = flag;
    }

  public:
    static void setup();
    static bool open(dword id, dword size);
    static bool open(dword id, dword size, dword pid, dword linearAddress);
    static bool attach(dword id, struct Process* process, LinearAddress address);
    static bool detach(dword id, struct Process* process);
    static SharedMemoryObject* find(dword id);
    enum
    {
        UN_MAPPED = -1,
        FLAG_NOT_SHARED = 0x01,
    };

  private:
    dword id_;
    dword size_;
    int attachedCount_;
    int physicalPageCount_;
    int* physicalPages_;
    dword* flags_;
};

class Segment {

  public:
    virtual ~Segment(){};
    virtual bool faultHandler(LinearAddress address, dword error) = 0;

    inline virtual int getErrorNumber() {
        return errorNumber_;
    }

    inline virtual LinearAddress getStart() {
        return start_;
    }

    inline virtual dword getSize() {
        return size_;
    }

    inline virtual bool inRange(LinearAddress address) {

        return (address >= start_ && address < start_ + size_);
    }

  protected:
    LinearAddress start_;
    dword         size_;
    byte errorNumber_;

  public:
    static const byte FAULT_STACK_OVERFLOW = 0x01;
    static const byte FAULT_OUT_OF_RANGE   = 0x02;
    static const byte FAULT_UNKNOWN        = 0x03;
};

class StackSegment : public Segment {

  public:
    StackSegment(LinearAddress start, dword size);
    virtual ~StackSegment();
    virtual bool inRange(LinearAddress address) {
        return (start_ - size_ <= address && address <= start_);
    }

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
};

class HeapSegment : public Segment {

  public:
    HeapSegment(LinearAddress start, dword size);
    virtual ~HeapSegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
};

class SharedMemorySegment : public Segment {

  public:
    SharedMemorySegment();
    SharedMemorySegment(LinearAddress start, dword size, SharedMemoryObject* sharedMemoryObject, bool writable = true);
    virtual ~SharedMemorySegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
    inline virtual dword getId() const {
        return sharedMemoryObject_->getId();
    }

    inline virtual void setWritable(bool writable) {
        writable_ = writable;
    }

  public:
    static SharedMemorySegment* find(Process* process, dword id);

  protected:
    SharedMemoryObject* sharedMemoryObject_;
    bool writable_;
};

#endif
