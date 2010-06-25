/*----------------------------------------------------------------------
    Stream
----------------------------------------------------------------------*/

#ifndef _MONAPI_STREAM_
#define _MONAPI_STREAM_

#include <sys/types.h>
#include <sys/error.h>
#include <monapi/Mutex.h>

namespace MonAPI {

class Stream
{
public:
    Stream();
    Stream(uint32_t size, uint32_t handle = 0);
    virtual ~Stream();

    bool isInvalid() const { return M_OK != lastError_; }
    intptr_t getLastError() const { return lastError_; }

public:
    static Stream* FromHandle(uint32_t handle);
    uint32_t write(uint8_t* buffer, uint32_t size);
    uint32_t read(uint8_t* buffer, uint32_t size);
    void waitForWrite();
    void waitForRead();
    int lockForRead();
    int unlockForRead();
    int tryLockForRead();
    int lockForWrite();
    int unlockForWrite();
    int tryLockForWrite();
    uint32_t size() const;
    uint32_t capacity() const;
    uint32_t handle() const { return memoryHandle_; }

protected:
    enum
    {
        MAX_WAIT_THREADS_NUM = 4
    };

    typedef struct
    {
        uint32_t size;
        uint32_t capacity;
        mutex_t accessMutexHandle;
        mutex_t readMutexHandle;
        mutex_t writeMutexHandle;
        uint32_t waitForReadThreads[MAX_WAIT_THREADS_NUM];
        uint32_t waitForWriteThreads[MAX_WAIT_THREADS_NUM];
    } StreamHeader;

    bool initialize(uint32_t size);
    bool initializeFromHandle(uint32_t handle);
    void setWaitForRead();
    void setWaitForWrite();

    uint32_t memoryHandle_;
    void* memoryAddress_;
    StreamHeader* header_;
    Mutex* access_;
    Mutex* readAccess_;
    Mutex* writeAccess_;
    intptr_t lastError_;
};

}; // namespace MonAPI

#endif // __MONAPI_STREAM_H__
