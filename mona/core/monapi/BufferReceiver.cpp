#include <monapi/Message.h>
#include <monapi/string.h>

namespace MonAPI {

bool BufferReceiver::receive(const void* source, uintptr_t maxSourceSize)
{
    uintptr_t sizeToReceive = maxSourceSize > restSizeToReceive() ? restSizeToReceive() : maxSourceSize;
    memcpy(buffer_ + receivedSize_, source, sizeToReceive);
    receivedSize_ += sizeToReceive;
}

}
