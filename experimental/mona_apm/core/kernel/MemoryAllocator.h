#include <sys/types.h>
#include "PageManager.h"

typedef void* mspace;

extern "C"
{
    mspace create_mspace_with_base(void* base, size_t capacity, int locked);
    void* mspace_malloc(mspace msp, size_t bytes);
    void mspace_free(mspace msp, void* mem);
}


class MemoryAllocator
{
public:
    MemoryAllocator(LinearAddress address, dword size);

public:
    void* Allocate(dword size);
    void Free(void* address);

protected:
    mspace base;
};
