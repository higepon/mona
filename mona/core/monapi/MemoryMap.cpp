/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

const uint32_t MemoryMap::START_ADDRESS = 0x90000000;
const uint32_t MemoryMap::MAX_SIZE      = 0x10000000;
uint32_t MemoryMap::nextAddress;
uint32_t MemoryMap::lastError;

//#define TRACE_MEMORY_MAP

#ifdef TRACE_MEMORY_MAP
#define MEMORY_MAP_TRACE(...)  _logprintf(__VA_ARGS__), _logprintf("  %s %s:%d:(%s)\n", MonAPI::System::getProcessInfo()->name, __FILE__, __LINE__, __func__) ;
#else
#define MEMORY_MAP_TRACE(...) /* */
#endif

void MemoryMap::initialize()
{
    nextAddress = START_ADDRESS;
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
    uint32_t size = syscall_memory_map_get_size(id);

    if (size == 0)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_MEMORY_MAP_NOT_FOUND;
        return NULL;
    }

    if (nextAddress + size > START_ADDRESS + MAX_SIZE)
    {
        MEMORY_MAP_TRACE("");
        lastError = ERROR_NOT_ENOUGH_ADDRESS_SPACE;
        return NULL;
    }

  uint32_t tid = syscall_get_tid();
  if (78 == tid) _logprintf("%s:%d next Address = %x &=%x\n", __FILE__, __LINE__, nextAddress, &nextAddress);
    if (syscall_memory_map_map(id, nextAddress))
    {
  if (78 == tid) _logprintf("%s:%d next Address = %x &=%x\n", __FILE__, __LINE__, nextAddress, &nextAddress);
        MEMORY_MAP_TRACE("");
        lastError = ERROR_NOT_MAP_ATATCH_ERROR;
        return NULL;
    }
    if (78 == tid) _logprintf("%s:%d next Address = %x &=%x\n", __FILE__, __LINE__, nextAddress, &nextAddress);

    uint8_t* result = (uint8_t*)(nextAddress);
    if (78 == tid) _logprintf("%s:%d next Address = %x &=%x\n", __FILE__, __LINE__, nextAddress, &nextAddress);
    nextAddress += size;
  if (78 == tid) _logprintf("%s:%d next Address = %x\n", __FILE__, __LINE__, nextAddress);
    if (78 == tid) _logprintf("%s:%d next Address = %x\n", __FILE__, __LINE__, result);
    return result;
}

bool MemoryMap::unmap(uint32_t id)
{
    if (syscall_memory_map_unmap(id))
    {
        lastError = 6;
        return false;
    }

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

int monapi_cmemorymap_unmap(uint32_t id)
{
    return MonAPI::MemoryMap::unmap(id) ? 1 : 0;
}

