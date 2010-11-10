#ifndef _MONAPI_SHARED_MEMORY_
#define _MONAPI_SHARED_MEMORY_

#include <sys/types.h>

namespace MonAPI {

class SharedMemory
{
public:
    SharedMemory(uintptr_t size) :
        size_(size),
        isMapped_(false),
        handle_(0),
        data_(NULL)
    {
    }

    SharedMemory(uintptr_t handle, uintptr_t size) :
        size_(size),
        isMapped_(false),
        handle_(handle),
        data_(NULL)
    {
    }

    virtual ~SharedMemory()
    {
        if (isMapped_) {
            unmap();
        }
    }

    bool isMapped() const
    {
        return isMapped_;
    }

    uintptr_t handle() const
    {
        return handle_;
    }

    uint8_t* data() const
    {
        return data_;
    }

    uintptr_t size() const
    {
        return size_;
    }

    intptr_t map(bool isImmediateMap = false)
    {
        if (isMapped_) {
            return M_BUSY;
        }
        if (handle_ == 0) {
            handle_ = MonAPI::MemoryMap::create(size_);
            if (0 == handle_) {
                return M_MEMORY_MAP_ERROR;
            }
        }
        data_ = MonAPI::MemoryMap::map(handle_, isImmediateMap);
        if (NULL == data_) {
            handle_ = 0;
            return M_MEMORY_MAP_ERROR;
        }
        isMapped_ = true;
        return M_OK;
    }

    intptr_t unmap()
    {
        if (MonAPI::MemoryMap::unmap(handle_)) {
            isMapped_ = false;
            handle_ = 0;
            data_ = NULL;
            return M_OK;
        } else {
            _logprintf("Warning: SharedMemory::unmap failed handle=%x size=%x", handle(), size());
            return M_MEMORY_MAP_ERROR;
        }
    }

private:
    uintptr_t size_;
    bool isMapped_;
    uintptr_t handle_;
    uint8_t* data_;
};

}

#endif
