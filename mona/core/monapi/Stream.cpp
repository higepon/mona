#include <monapi/Mutex.h>
#include <monapi.h>
using namespace MonAPI;

#define CHECK_CORRUPT_AND_DIE() checkCorruptAndDie(__FILE__, __LINE__)

Stream::Stream() : access_(NULL), lastError_(M_OK)
{
    const int DEFAULT_STREAM_SIZE_BYTE = 64 * 1024;
    if (!initialize(DEFAULT_STREAM_SIZE_BYTE)) {
        lastError_ = M_BAD_STREAM;
    }
}

Stream::Stream(uint32_t size, uint32_t handle /* = 0 */) : access_(NULL), lastError_(M_OK)
{
    if (handle != 0) {
        if (!initializeFromHandle(handle)) {
            lastError_ = M_BAD_STREAM;
        }
    } else {
        if (!initialize(size)) {
            lastError_ = M_BAD_STREAM;
        }
    }
}

Stream::~Stream()
{
    if (memoryHandle_ != 0) {
        MemoryMap::unmap(memoryHandle_);
    }
    if (access_ != NULL) {
        delete access_;
    }
}

Stream* Stream::createFromHandle(uint32_t handle)
{
    return new Stream(0, handle);
}

uint32_t Stream::write(const void* buffer, uint32_t size, bool waitsOnFull /* = false */)
{
    uint32_t sizeWritten = writeInternal(buffer, size);
    if (sizeWritten == 0 && waitsOnFull) {
        waitForWrite();
        return writeInternal(buffer, size);
    } else {
        return sizeWritten;
    }
}

uint32_t Stream::writeInternal(const void* buffer, uint32_t size)
{
    access_->lock();
    uint32_t memorySize = header_->size;
    uint32_t memoryCapacity = header_->capacity;
    uint32_t freeSize = memoryCapacity - memorySize;
    uint32_t writeSize;
    if (0 == freeSize || size <= 0) {
        writeSize = 0;
    } else if (size < freeSize) {
        memcpy((void*)((uint32_t)memoryAddress_ + memorySize), buffer, size);
        CHECK_CORRUPT_AND_DIE();
        header_->size = memorySize + size;
        writeSize = size;
    } else {
        memcpy((void*)((uint32_t)memoryAddress_ + memorySize), buffer, freeSize);
        CHECK_CORRUPT_AND_DIE();
        header_->size = memoryCapacity;
        writeSize = freeSize;
    }

    if (memorySize != 0) {
        access_->unlock();
        CHECK_CORRUPT_AND_DIE();
        return writeSize;
    }

    // for writeSize == 0, we just notify. It means EOF(ctrl-D).
    if (writeSize >= 0) {
        uint32_t waitingThread = header_->waitForReadThread;
        header_->waitForReadThread = THREAD_UNKNOWN;
        access_->unlock();
        if (waitingThread != THREAD_UNKNOWN) {
            if (Message::send(waitingThread, MSG_READ_READY) != M_OK) {
                monapi_warn("waiting process is gone\n");
            }
        }
    }
    return writeSize;
}

uint32_t Stream::read(void* buffer, uint32_t size, bool waitsOnEmpty /* = false */)
{
    uint32_t sizeRead = readInternal(buffer, size);
    if (sizeRead == 0 && waitsOnEmpty) {
        _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        waitForRead();
        _logprintf("after waitForRead %s %s:%d\n", __func__, __FILE__, __LINE__);
        return readInternal(buffer, size);
    } else {
        return sizeRead;
    }
}

uint32_t Stream::readInternal(void* buffer, uint32_t size)
{
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    access_->lock();
    uint32_t memorySize = header_->size;
    uint32_t readSize;
    if (0 == memorySize || size <= 0) {
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        readSize = 0;
    } else if (size < memorySize) {
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        memcpy(buffer, memoryAddress_, size);
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        CHECK_CORRUPT_AND_DIE();
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        memmove(memoryAddress_, (uint8_t*)((uint32_t)memoryAddress_ + size), memorySize - size);
        CHECK_CORRUPT_AND_DIE();
        header_->size = memorySize - size;
        readSize = size;
    } else {
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        memcpy(buffer, memoryAddress_, memorySize);
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        CHECK_CORRUPT_AND_DIE();
        header_->size = 0;
        readSize = memorySize;
    }
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    if (readSize ==  0) {
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
        access_->unlock();
        CHECK_CORRUPT_AND_DIE();
        return readSize;
    }
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    uint32_t waitingThread = header_->waitForWriteThread;
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    header_->waitForWriteThread = THREAD_UNKNOWN;
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    access_->unlock();
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    if (waitingThread != THREAD_UNKNOWN) {
        if (Message::send(waitingThread, MSG_WRITE_READY) != M_OK) {
            monapi_warn("waiting process is gone\n");
        }
    }
    _logprintf("%s %s:%d thread(%d)\n", __func__, __FILE__, __LINE__, MonAPI::System::getThreadID());
    CHECK_CORRUPT_AND_DIE();
    return readSize;
}

void Stream::waitMessage(uint32_t messageHeaderToWait)
{
    MessageInfo msg;
    for (int i = 0; ; i++) {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != M_OK) {
            i--;
            syscall_mthread_yield_message();
        } else if (msg.header == messageHeaderToWait) {
            if (Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                monapi_warn("peek error");
            }
            return;
        }
    }
}

void Stream::waitForWrite()
{
    access_->lock();
    if (header_->capacity != header_->size) {
        access_->unlock();
        return;
    }
    setWaitForWrite();
    access_->unlock();
    waitMessage(MSG_WRITE_READY);
}

void Stream::waitForRead()
{
    access_->lock();
    if (header_->size != 0) {
        access_->unlock();
        return;
    }
    setWaitForRead();
    access_->unlock();
    waitMessage(MSG_READ_READY);
}

uint32_t Stream::size() const
{
    access_->lock();
    uint32_t size = header_->size;
    access_->unlock();
    return size;
}

uint32_t Stream::capacity() const
{
    access_->lock();
    uint32_t capacity = header_->capacity;
    access_->unlock();
    return capacity;
}

/*----------------------------------------------------------------------
    Protected functions
----------------------------------------------------------------------*/
bool Stream::initialize(uint32_t size)
{
    access_ = new Mutex();
    memoryHandle_ = MemoryMap::create(size + sizeof(StreamHeader));
    if (0 == memoryHandle_)
    {
        monapi_warn("Stream: MemoryMap initialize error %d", MemoryMap::getLastError());
        return false;
    }
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address) {
        monapi_warn("Stream: MemoryMap map() error %d", MemoryMap::getLastError());
        return false;
    }
    header_ = (StreamHeader*)address;
    header_->size = 0;
    header_->capacity = size;
    header_->accessMutexHandle = access_->getMutex_t();
    header_->waitForReadThread = THREAD_UNKNOWN;
    header_->waitForWriteThread = THREAD_UNKNOWN;
    header_->detectCorrupt = DETECT_CORRUPT_MAGIC;
    memoryAddress_ = (void*)((uint32_t)address + sizeof(StreamHeader));
    return true;
}

bool Stream::initializeFromHandle(uint32_t handle)
{
    memoryHandle_ = handle;
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address) {
        monapi_warn("Stream: MemoryMap error handle=%x %d\n", handle, MemoryMap::getLastError());
        return false;
    }
    header_ = (StreamHeader*)address;
    access_ = new Mutex(&(header_->accessMutexHandle));
    if (access_->getLastError() != M_OK) {
        monapi_warn("mutex access initalize failed %s:%d error code=%d\n", __FILE__, __LINE__, access_->getLastError());
        return false;
    }
    memoryAddress_ = (void*)((uint32_t)address + sizeof(StreamHeader));
    return true;
}

void Stream::setWaitForRead()
{
    if (header_->waitForReadThread == THREAD_UNKNOWN) {
        header_->waitForReadThread = System::getThreadID();
    } else {
        monapi_fatal("setWaitForRead max reached exit \n");
    }
}

void Stream::setWaitForWrite()
{
    if (header_->waitForWriteThread == THREAD_UNKNOWN) {
        header_->waitForWriteThread = System::getThreadID();
    } else {
        monapi_fatal("setWaitForWrite max reached exit \n");
    }
}

uint8_t* Stream::header() const
{
    return (uint8_t*)header_;
}
