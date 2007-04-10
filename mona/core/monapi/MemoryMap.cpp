/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

const uint32_t MemoryMap::START_ADDRESS = 0x90000000;
const uint32_t MemoryMap::MAX_SIZE      = 0x10000000;
uint32_t MemoryMap::nextAddress;
uint32_t MemoryMap::lastError;

void MemoryMap::initialize()
{
    nextAddress = START_ADDRESS;
    lastError = 0;
}

uint32_t MemoryMap::create(uint32_t size)
{
    /* error */
    if (size <= 0)
    {
        lastError = 1;
        return 0;
    }

    size = (size + 4095) & 0xFFFFF000; /* nikq */

    uint32_t result = syscall_memory_map_create(size);

    /* error */
    if (result == 0)
    {
        lastError = 2;
        return 0;
    }

    return result;
}

uint8_t* MemoryMap::map(uint32_t id)
{
    /* to be first fit */

    uint32_t size = syscall_memory_map_get_size(id);

    if (size == 0)
    {
        lastError = 3;
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        return NULL;
    }

    if (nextAddress + size > START_ADDRESS + MAX_SIZE)
    {
        lastError = 4;
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        return NULL;
    }

    if (syscall_memory_map_map(id, nextAddress))
    {
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        lastError = 5;
        return NULL;
    }

    uint8_t* result = (uint8_t*)(nextAddress);
    nextAddress += size;
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
