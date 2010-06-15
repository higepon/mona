#ifndef _MONA_MEMORY_ALLOCATOR_
#define _MONA_MEMORY_ALLOCATOR_

#include <sys/types.h>
#include "PageManager.h"

typedef void* mspace;

extern "C"
{
    mspace create_mspace_with_base(void* base, size_t capacity, int locked);
    void* mspace_malloc(mspace msp, size_t uint8_ts);
    void mspace_free(mspace msp, void* mem);
}


class MemoryAllocator
{
public:
    MemoryAllocator(LinearAddress address, uint32_t size);

public:
    void* Allocate(uint32_t size);
    void Free(void* address);

protected:
    mspace base;
};

#endif
