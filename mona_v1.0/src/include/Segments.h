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

#include <PageManager.h>
#include <operator.h>
#include <kernel.h>
#include <string.h>
#include <Queue.h>
#include <Process.h>

class SharedMemoryObject : public Queue {

  public:
    SharedMemoryObject();
    SharedMemoryObject(dword id, dword size);
    virtual ~SharedMemoryObject();

    inline virtual int getAttachedCount() const {return attachedCount_;}
    inline virtual void setAttachedCount(int count) {attachedCount_ = count;}
    inline virtual dword getId() const {return id_;}
    inline virtual dword getSize() const {return size_;}
    inline virtual int isMapped(int physicalIndex) const {

        if (physicalIndex >= physicalPageCount_) return UN_MAPPED;
        return physicalPages_[physicalIndex];
    }

    inline virtual void map(int physicalIndex, PhysicalAddress address) {

        if (physicalIndex >= physicalPageCount_) return;

        physicalPages_[physicalIndex] = address;
    }

  public:
    static void setup();
    static bool open(dword id, dword size);
    static bool attach(dword id, struct ProcessInfo* process, LinearAddress address);
    static bool detach(dword id, struct ProcessInfo* process);

    static const int UN_MAPPED = -1;

  private:
    static SharedMemoryObject* find(dword id);

  private:
    dword id_;
    dword size_;
    int attachedCount_;
    int physicalPageCount_;
    int* physicalPages_;
};

class Segment {

  public:
    virtual bool faultHandler(LinearAddress address, dword error) = 0;
    virtual int getErrorNumber() {return errorNumber_;}
    virtual LinearAddress getStart() {return start_;}
    virtual dword getSize() {return size_;}
    virtual bool inRange(LinearAddress address) {

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
    StackSegment(LinearAddress start, dword initileSize, dword maxSize);
    virtual ~StackSegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
    virtual bool inRange(LinearAddress address) {

        return (address >= start_ - PageManager::ARCH_PAGE_SIZE && address <= start_);
    }

  private:
    bool tryExtend(LinearAddress address);
    bool allocatePage(LinearAddress address);

  protected:
    bool isAutoExtend_;
    dword maxSize_;

};

class HeapSegment : public Segment {

  public:
    HeapSegment(LinearAddress start, dword size);
    virtual ~HeapSegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
};

class SharedMemorySegment : public Segment, public Queue {

  public:
    SharedMemorySegment();
    SharedMemorySegment(LinearAddress start, dword size, SharedMemoryObject* sharedMemoryObject);
    virtual ~SharedMemorySegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
    virtual dword getId() const {return sharedMemoryObject_->getId();}

  public:
    static SharedMemorySegment* find(SharedMemorySegment* head, dword id);

  protected:
    SharedMemoryObject* sharedMemoryObject_;
};

#endif
