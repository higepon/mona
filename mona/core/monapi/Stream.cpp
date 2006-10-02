#include <monapi.h>

using namespace MonAPI;

Stream::Stream()
{
    // default stream size is 64KB.
    initialize(64 * 1024);
}

Stream::Stream(dword size, dword handle /* = 0 */)
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
    if (access_ != NULL)
    {
        delete access_;
    }
}

Stream* Stream::FromHandle(dword handle)
{
    return new Stream(0, handle);
}

dword Stream::write(byte* buffer, dword size)
{
//    if (-1 == access_->tryLock()) return 0;
    access_->lock();
    dword memorySize = header_->size;
    dword memoryCapacity = header_->capacity;
    dword freeSize = memoryCapacity - memorySize;
    dword writeSize;
    if (0 == freeSize || size <= 0)
    {
        writeSize = 0;
    }
    else if (size < freeSize)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        memcpy((void*)((dword)memoryAddress_ + memorySize), buffer, size);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        header_->size = memorySize + size;
        writeSize = size;
    }
    else
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        memcpy((void*)((dword)memoryAddress_ + memorySize), buffer, freeSize);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        header_->size = memoryCapacity;
        writeSize = freeSize;
    }

    if (memorySize != 0)
    {
        access_->unlock();
        return writeSize;
    }

    dword* threads = new dword[MAX_WAIT_THREADS_NUM];
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    memcpy(threads, header_->waitForReadThreads, sizeof(dword) * MAX_WAIT_THREADS_NUM);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForReadThreads[i] = THREAD_UNKNOWN;
    }
    access_->unlock();

    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        dword thread = threads[i];
        if (THREAD_UNKNOWN == thread) continue;
        Message::send(thread, MSG_READ_MEMORY_READY);
    }
    delete[] threads;
    return writeSize;
}

dword Stream::read(byte* buffer, dword size)
{
//    if (-1 == access_->tryLock()) return 0;
    access_->lock();
    dword memorySize = header_->size;
    dword readSize;
    if (0 == memorySize || size <= 0)
    {
        readSize = 0;
    }
    else if (size < memorySize)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        memcpy(buffer, memoryAddress_, size);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        memcpy(memoryAddress_, (byte*)((dword)memoryAddress_ + readSize), memorySize - size);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        header_->size = memorySize - size;
        readSize = size;
    }
    else
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        memcpy(buffer, memoryAddress_, memorySize);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        header_->size = 0;
        readSize = memorySize;
    }

    if (readSize ==  0)
    {
        access_->unlock();
        return readSize;
    }

    dword* threads = new dword[MAX_WAIT_THREADS_NUM];
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    memcpy(threads, header_->waitForWriteThreads, sizeof(dword) * MAX_WAIT_THREADS_NUM);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForWriteThreads[i] = THREAD_UNKNOWN;
    }
    access_->unlock();
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        dword thread = threads[i];
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

dword Stream::size() const
{
    access_->lock();
    dword size = header_->size;
    access_->unlock();
    return size;
}

dword Stream::capacity() const
{
    access_->lock();
    dword capacity = header_->capacity;
    access_->unlock();
    return capacity;
}

/*----------------------------------------------------------------------
    Protected functions
----------------------------------------------------------------------*/
bool Stream::initialize(dword size)
{
    access_ = new Mutex();
    memoryHandle_ = MemoryMap::create(size + sizeof(StreamHeader));
    if (0 == memoryHandle_)
    {
        printf("Stream: MemoryMap error\n");
        return false;
    }
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address)
    {
        printf("Stream: MemoryMap error\n");
        return false;
    }
    header_ = (StreamHeader*)address;
    header_->size = 0;
    header_->capacity = size;
    header_->mutexHandle = access_->getId();
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForReadThreads[i] = THREAD_UNKNOWN;
    }
    for (int i = 0; i < MAX_WAIT_THREADS_NUM; i++)
    {
        header_->waitForWriteThreads[i] = THREAD_UNKNOWN;
    }

    memoryAddress_ = (void*)((dword)address + sizeof(StreamHeader));

    return true;
}

bool Stream::initializeFromHandle(dword handle)
{
    memoryHandle_ = handle;
    access_ = new Mutex();
    void* address = MemoryMap::map(memoryHandle_);
    if (NULL == address)
    {
        printf("Stream: MemoryMap error\n");
        return false;
    }
    header_ = (StreamHeader*)address;
    access_ = new Mutex(header_->mutexHandle);
    memoryAddress_ = (void*)((dword)address + sizeof(StreamHeader));
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
    printf("setWaitForRead max reached exit \n");
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
    printf("setWaitForWrite max reached exit \n");
    exit(-1);
}
