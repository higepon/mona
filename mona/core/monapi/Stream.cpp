#include <monapi.h>
using namespace MonAPI;

Stream::Stream()
{
    // default stream size is 64KB.
    initialize(64 * 1024);
}

Stream::Stream(uint32_t size, uint32_t handle /* = 0 */)
{
    if (handle != 0)
    {
        initializeFromHandle(handle);
    }
    else
    {
        initialize(size);
    }
}

Stream::~Stream()
{
    if (memoryHandle_ != 0)
    {
        MemoryMap::unmap(memoryHandle_);
    }
    if (access_ != NULL) delete access_;
    if (readAccess_ != NULL) delete readAccess_;
    if (writeAccess_ != NULL) delete writeAccess_;

}

extern void* g_msp;
Stream* Stream::FromHandle(uint32_t handle)
{
    return new Stream(0, handle);
}

uint32_t Stream::write(uint8_t* buffer, uint32_t size)
{
    access_->lock();
    uint32_t memorySize = header_->size;
    uint32_t memoryCapacity = header_->capacity;
    uint32_t freeSize = memoryCapacity - memorySize;
    uint32_t writeSize;
    if (0 == freeSize || size <= 0)
    {
        writeSize = 0;
    }
    else if (size < freeSize)
    {
        PsInfo pi = *MonAPI::System::getProcessInfo();
//         _logprintf("%s:%d:%x [%s]\n", __func__, __LINE__, System::getThreadID(), pi.name);
//         _logprintf("this=%x header_ = %x :memorySize = %x:memoryAddress_=%x: dest=%x:buffer=%x:size=%x\n"
//                    , this
//                    , header_
//                    , memorySize
//                    , memoryAddress_
//                    , (void*)((uint32_t)memoryAddress_ + memorySize)
//                    , buffer
//                    , size);
        memcpy((void*)((uint32_t)memoryAddress_ + memorySize), buffer, size);
//        _logprintf("%s:%d memorySize from %x to %x\n", __FILE__, __LINE__, header_->size, memorySize + size);
        header_->size = memorySize + size;
        writeSize = size;
    }
    else
    {
        memcpy((void*)((uint32_t)memoryAddress_ + memorySize), buffer, freeSize);
        //_logprintf("%s:%d memorySize from %x to %x\n", __FILE__, __LINE__, header_->size, memoryCapacity);
        header_->size = memoryCapacity;
        writeSize = freeSize;
    }
    if (memorySize != 0)
    {
        access_->unlock();
        return writeSize;
    }
    uint32_t* threads = new uint32_t[MAX_WAIT_THREADS_NUM];
    memcpy(threads, header_->waitForReadThreads, sizeof(uint32_t) * MAX_WAIT_THREADS_NUM);
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForReadThreads[i] = THREAD_UNKNOWN;
    }
    access_->unlock();
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        uint32_t thread = threads[i];
        if (THREAD_UNKNOWN == thread) continue;
        Message::send(thread, MSG_READ_MEMORY_READY);
    }
    delete[] threads;
    return writeSize;
}

uint32_t Stream::read(uint8_t* buffer, uint32_t size)
{
//    if (-1 == access_->tryLock()) return 0;
    access_->lock();
//    printf("[read] header->size=%d header->capacity=%d size=%d\n", header_->size, header_->capacity, size);
    uint32_t memorySize = header_->size;
    uint32_t readSize;
    if (0 == memorySize || size <= 0)
    {
        readSize = 0;
    }
    else if (size < memorySize)
    {
        memcpy(buffer, memoryAddress_, size);
        memcpy(memoryAddress_, (uint8_t*)((uint32_t)memoryAddress_ + size), memorySize - size);
        header_->size = memorySize - size;
        readSize = size;
    }
    else
    {
        memcpy(buffer, memoryAddress_, memorySize);
        header_->size = 0;
        readSize = memorySize;
    }
//    printf("[read2] header->size=%d header->capacity=%d size=%d", header_->size, header_->capacity, size);
    if (readSize ==  0)
    {
        access_->unlock();
        return readSize;
    }

    uint32_t* threads = new uint32_t[MAX_WAIT_THREADS_NUM];
    memcpy(threads, header_->waitForWriteThreads, sizeof(uint32_t) * MAX_WAIT_THREADS_NUM);
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForWriteThreads[i] = THREAD_UNKNOWN;
    }
    access_->unlock();
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        uint32_t thread = threads[i];
        if (THREAD_UNKNOWN == thread) continue;
        Message::send(thread, MSG_WRITE_MEMORY_READY);
    }
    delete[] threads;
    return readSize;
}

void Stream::waitForWrite()
{
    access_->lock();
    if (header_->capacity != header_->size)
    {
        access_->unlock();
        return;
    }
    setWaitForWrite();
    access_->unlock();

    MessageInfo msg;
    for (int i = 0; ; i++)
    {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != 0)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (msg.header == MSG_WRITE_MEMORY_READY)
        {
            MonAPI::Message::peek(&msg, i, PEEK_REMOVE);
            return;
        }
    }
}

void Stream::waitForRead()
{
    access_->lock();
    if (header_->size != 0)
    {
        access_->unlock();
        return;
    }
    setWaitForRead();
    access_->unlock();

    MessageInfo msg;
    for (int i = 0; ; i++)
    {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != 0)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (msg.header == MSG_READ_MEMORY_READY)
        {
            MonAPI::Message::peek(&msg, i, PEEK_REMOVE);
            return;
        }
    }
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
//    _logprintf("%s:%d", __func__, __LINE__);
    access_ = new Mutex();
    readAccess_ = new Mutex();
    writeAccess_ = new Mutex();
    memoryHandle_ = MemoryMap::create(size + sizeof(StreamHeader));
    if (0 == memoryHandle_)
    {
        _printf("Stream: MemoryMap initialize error %s at %s \n", MemoryMap::getLastErrorString(), System::getProcessInfo()->name);
        return false;
    }
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address)
    {
        _printf("Stream: MemoryMap map() error %s at %s\n", MemoryMap::getLastErrorString(), System::getProcessInfo()->name);
        return false;
    }
    header_ = (StreamHeader*)address;
    header_->size = 0;
    header_->capacity = size;
    header_->accessMutexHandle = access_->getId();
    header_->readMutexHandle = readAccess_->getId();
    header_->writeMutexHandle = writeAccess_->getId();
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForReadThreads[i] = THREAD_UNKNOWN;
    }
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForWriteThreads[i] = THREAD_UNKNOWN;
    }

    memoryAddress_ = (void*)((uint32_t)address + sizeof(StreamHeader));

    return true;
}

bool Stream::initializeFromHandle(uint32_t handle)
{
    PsInfo pi = *MonAPI::System::getProcessInfo();
//    _logprintf("initializeFromHandle %s:%d[%s]\n", __func__, __LINE__, pi.name);

    memoryHandle_ = handle;
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address)
    {
        _printf("Stream: MemoryMap error handle=%x %s at %s \n", handle, MemoryMap::getLastErrorString(),  System::getProcessInfo()->name);
        return false;
    }
    header_ = (StreamHeader*)address;
//    _logprintf("this = %x, header_=%x\n", this, header_);
    access_ = new Mutex(header_->accessMutexHandle);
    readAccess_ = new Mutex(header_->readMutexHandle);
    writeAccess_ = new Mutex(header_->writeMutexHandle);
    memoryAddress_ = (void*)((uint32_t)address + sizeof(StreamHeader));
//    _logprintf("memoryAddress_=%x\n", memoryAddress_);
//    _logprintf("memorySize = %x\n", header_->size);

    return true;
}

void Stream::setWaitForRead()
{
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        if (header_->waitForReadThreads[i] == THREAD_UNKNOWN)
        {
            header_->waitForReadThreads[i] = System::getThreadID();
            return;
        }
    }
    _printf("setWaitForRead max reached exit \n");
    exit(-1);
}

void Stream::setWaitForWrite()
{
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        if (header_->waitForWriteThreads[i] == THREAD_UNKNOWN)
        {
            header_->waitForWriteThreads[i] = System::getThreadID();
            return;
        }
    }
    _printf("setWaitForWrite max reached exit \n");
    exit(-1);
}

int Stream::lockForRead()
{
    return readAccess_->lock();
}

int Stream::unlockForRead()
{
    return readAccess_->unlock();
}

int Stream::tryLockForRead()
{
    return readAccess_->tryLock();
}

int Stream::lockForWrite()
{
    return writeAccess_->lock();
}

int Stream::unlockForWrite()
{
    return writeAccess_->unlock();
}

int Stream::tryLockForWrite()
{
    return writeAccess_->tryLock();
}

