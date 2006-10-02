/*----------------------------------------------------------------------
    Stream
----------------------------------------------------------------------*/

#ifndef __MONAPI_STREAM_H__
#define __MONAPI_STREAM_H__

#include <sys/types.h>

namespace MonAPI {

class Stream
{
public:
    Stream();
    Stream(dword size, dword handle = 0);
    virtual ~Stream();

public:
    static Stream* FromHandle(dword handle);
    dword write(byte* buffer, dword size);
    dword read(byte* buffer, dword size);
    void waitForWrite();
    void waitForRead();
    dword size() const;
    dword capacity() const;
    dword handle() const { return memoryHandle_; }

protected:
    enum
    {
        MAX_WAIT_THREADS_NUM = 4
    };

    typedef struct
    {
        dword size;
        dword capacity;
        dword mutexHandle;
        dword waitForReadThreads[MAX_WAIT_THREADS_NUM];
        dword waitForWriteThreads[MAX_WAIT_THREADS_NUM];
    } StreamHeader;

    bool initialize(dword size);
    bool initializeFromHandle(dword handle);
    void setWaitForRead();
    void setWaitForWrite();

    dword memoryHandle_;
    void* memoryAddress_;
    StreamHeader* header_;
    Mutex* access_;

};

}; // namespace MonAPI

#endif // __MONAPI_STREAM_H__
