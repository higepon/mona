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

class Segment {

  public:
    virtual bool faultHandler(LinearAddress address, dword error) = 0;
    virtual int getErrorNumber() {return errorNumber_;}
    virtual LinearAddress getStart() {return start_;}
    virtual dword getSize() {return size_;}

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

class SharedMemoryObject {

  public:
    SharedMemoryObject(dword id, dword size) {

        /* check dup */


        physicalPageCount_ = size / 4096;
        physicalPages_     = new int[physicalPageCount_];

        if (physicalPages_ == NULL) panic("SharedMemoryObject: new failed");

        memset(physicalPages_, UN_MAPPED, sizeof(int) * physicalPageCount_);


    }
    virtual ~SharedMemoryObject();

  public:
    static bool open(dword id, dword size);
    static bool attach(dword id, PageEntry* directory, LinearAddress address);
    static bool detach(dword id, PageEntry* directory);

    static const int UN_MAPPED = -1;

  private:
    dword id_;
    dword size_;
    int attachedCount_;
    int physicalPageCount_;
    int* physicalPages_;

};


#endif
