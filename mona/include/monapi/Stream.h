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
    static Stream* createFromHandle(uint32_t handle);
    virtual ~Stream();

    bool isInvalid() const { return M_OK != lastError_; }
    intptr_t lastError() const { return lastError_; }
    uint32_t write(const void* buffer, uint32_t size, bool waitsOnFull = false);
    uint32_t read(void* buffer, uint32_t size, bool waitsOnEmpty = false);
    void waitForWrite();
    void waitForRead();
    uint32_t availableDataSize() const;
    uint32_t capacity() const;
    uint8_t* header() const;
    uint32_t handle() const { return memoryHandle_; }
protected:
    enum
    {
        DETECT_CORRUPT_MAGIC = 0x12334478,
        forbidden_comma
    };
    typedef struct
    {
        uint32_t size;
        uint32_t capacity;
        mutex_t accessMutexHandle;
        uint32_t waitForReadThread;
        uint32_t waitForWriteThread;
        uint32_t detectCorrupt;
    } StreamHeader;

    void checkCorruptAndDie(const char* file, int lineNo)
    {
        if (header_->detectCorrupt != DETECT_CORRUPT_MAGIC) {
            _logprintf("Warning:Stream is corrupted %s:%d", file, lineNo);
            uint8_t* p = (uint8_t*)0;
            *p = 0;
        }
    }

    uint32_t readInternal(void* buffer, uint32_t size);
    uint32_t writeInternal(const void* buffer, uint32_t size);
    bool initialize(uint32_t size);
    bool initializeFromHandle(uint32_t handle);
    void setWaitForRead();
    void setWaitForWrite();
    void waitMessage(uint32_t messageHeaderToWait);

    uint32_t memoryHandle_;
    void* memoryAddress_;
    StreamHeader* header_;
    Mutex* access_;
    intptr_t lastError_;
};

}; // namespace MonAPI

#endif // __MONAPI_STREAM_H__
