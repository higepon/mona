#ifndef _MONAPI_BUFFER_RECEIVER_
#define _MONAPI_BUFFER_RECEIVER_

#include <sys/types.h>

namespace MonAPI {

class BufferReceiver
{
public:
    BufferReceiver(uintptr_t bufferSize) : buffer_(new uint8_t[bufferSize]),
                                           bufferSize_(bufferSize),
                                           receivedSize_(0)
    {
    }

    ~BufferReceiver()
    {
        delete[] buffer_;
    }

    uintptr_t bufferSize() const
    {
        return bufferSize_;
    }

    bool isDone() const
    {
        return bufferSize_ == receivedSize_;
    }

    uint8_t* buffer() const
    {
        return buffer_;
    }

    uintptr_t restSizeToReceive() const
    {
        return bufferSize_ - receivedSize_;
    }

    bool receive(const void* source, uintptr_t maxSourceSize);

private:
    uint8_t* buffer_;
    uintptr_t bufferSize_;
    uintptr_t receivedSize_;
};


}

#endif
