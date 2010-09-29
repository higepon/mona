#ifndef _MONAPI_BUFFER_
#define _MONAPI_BUFFER_

namespace MonAPI {

/*
    class: Buffer

*/
class Buffer
{
public:
    Buffer() : data_(NULL), size_(0)
    {
    }

    Buffer(void* data, uintptr_t size) : data_((uint8_t*)data), size_(size)
    {
        ASSERT(data_);
    }

    virtual ~Buffer()
    {
        // we don't delete[] the data
        // caller should delete[].
    }

    uint8_t* data() const
    {
        return data_;
    }

    uintptr_t size() const
    {
        return size_;
    }

    void fillZero()
    {
        memset(data_, 0, size_);
    }

    static bool copy(Buffer& dest, uintptr_t destOffset, const Buffer& source, uintptr_t sourceOffset, uintptr_t size)
    {
        ASSERT(dest.data_);
        ASSERT(source.data_);
        if (dest.size() - destOffset < size) {
            monapi_warn("copy failed");
            return false;
        }
        if (source.size() - sourceOffset < size) {
            monapi_warn("copy failed");
            return false;
        }
        memcpy(dest.data_ + destOffset, source.data_ + sourceOffset, size);
        return true;
    }
private:
    uint8_t* data_;
    uintptr_t size_;
};

};

#endif
