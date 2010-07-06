/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#include <monapi.h>
#include <monapi/Mutex.h>

#define allocator_warn(x) monapi_warn("allocator : %s", x)

#include <sys/FirstFitAllocator.h>

namespace MonAPI {

const uint32_t MemoryMap::START_ADDRESS = 0x90000000;
const uint32_t MemoryMap::MAX_SIZE      = 0x10000000;
uint32_t MemoryMap::lastError;
FirstFitAllocator MemoryMap::allocator;
BinaryTree<void*> MemoryMap::addressMap;
Mutex MemoryMap::mutex;

//#define TRACE_MEMORY_MAP

#ifdef TRACE_MEMORY_MAP
#define MEMORY_MAP_TRACE(...)  _logprintf(__VA_ARGS__), _logprintf("  %s %s:%d:(%s)\n", MonAPI::System::getProcessInfo()->name, __FILE__, __LINE__, __func__) ;
#else
#define MEMORY_MAP_TRACE(...) /* */
#endif

void MemoryMap::initialize()
{
    allocator = FirstFitAllocator(START_ADDRESS, START_ADDRESS + MAX_SIZE);
    lastError = 0;
}

uint32_t MemoryMap::create(uint32_t size)
{
    MEMORY_MAP_TRACE("try to create size = %d", size);
    /* error */
    if (size <= 0)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_SIZE_ZERO;
        return 0;
    }

    size = (size + 4095) & 0xFFFFF000; /* nikq */

    uint32_t result = syscall_memory_map_create(size);

    /* error */
    if (result == 0)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_NOT_ENOUGH_KERNEL_MEMORY;
        return 0;
    }
    MEMORY_MAP_TRACE("create ok");
    return result;
}

uint8_t* MemoryMap::map(uint32_t id)
{
    /* to be first fit */
    MEMORY_MAP_TRACE("try to map() id = %x", id);
    uintptr_t size = syscall_memory_map_get_size(id);

    if (size == 0)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_MEMORY_MAP_NOT_FOUND;
        return NULL;
    }

    mutex.lock();
    void* address = allocator.allocate(size);
    if (address == NULL)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_NOT_ENOUGH_ADDRESS_SPACE;
        mutex.unlock();
        return NULL;
    }

    addressMap.add(id, address);

    if (syscall_memory_map_map(id, (uintptr_t)address))
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_NOT_MAP_ATATCH_ERROR;
        mutex.unlock();
        return NULL;
    }
    mutex.unlock();
    return (uint8_t*)address;
}

bool MemoryMap::unmap(uint32_t id)
{
    void* address = addressMap.get(id);
    if (NULL == address) {
        monapi_warn("bad map id");
    }

    if (syscall_memory_map_unmap(id))
    {
        lastError = M_MEMORY_MAP_ERROR;
        return false;
    }
    allocator.free(address);
    return true;
}

uint32_t MemoryMap::getLastError()
{
    return lastError;
}

uint32_t MemoryMap::getSize(uint32_t id)
{
    return syscall_memory_map_get_size(id);
}

const char* MemoryMap::getLastErrorString()
{
    switch (MemoryMap::getLastError())
    {
    case MemoryMap::ERROR_SIZE_ZERO:
        return "invalid size zero for memory map";
    case MemoryMap::ERROR_NOT_ENOUGH_KERNEL_MEMORY:
        return "not enough memory in kernel";
    case MemoryMap::ERROR_MEMORY_MAP_NOT_FOUND:
        return "memory map id not exist in kernel";
    case MemoryMap::ERROR_NOT_ENOUGH_ADDRESS_SPACE:
        return "process has not enough address space";
    case MemoryMap::ERROR_NOT_MAP_ATATCH_ERROR:
        return "attach memory map fail in kernel";
    default:
        return "unknown error";
    }
}

}

uint32_t monapi_cmemorymap_create(uint32_t size)
{
    return MonAPI::MemoryMap::create(size);
}

uint8_t* monapi_cmemorymap_map(uint32_t id)
{
    return MonAPI::MemoryMap::map(id);
}

intptr_t monapi_cmemorymap_unmap(uint32_t id)
{
    return MonAPI::MemoryMap::unmap(id) ? M_OK : M_MEMORY_MAP_ERROR;
}

