/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

const dword MemoryMap::START_ADDRESS = 0x90000000;
const dword MemoryMap::MAX_SIZE      = 0x10000000;
dword MemoryMap::nextAddress;
dword MemoryMap::lastError;

void MemoryMap::initialize()
{
    nextAddress = START_ADDRESS;
    lastError = 0;
}

dword MemoryMap::create(dword size)
{
    /* error */
    if (size <= 0)
    {
        lastError = 1;
        return 0;
    }

    size = (size + 4095) & 0xFFFFF000; /* nikq */

    dword result = syscall_memory_map_create(size);

    /* error */
    if (result == 0)
    {
        lastError = 2;
        return 0;
    }

    return result;
}

byte* MemoryMap::map(dword id)
{
    /* to be first fit */

    dword size = syscall_memory_map_get_size(id);

    if (size == 0)
    {
        lastError = 3;
        return NULL;
    }

    if (nextAddress + size > START_ADDRESS + MAX_SIZE)
    {
        lastError = 4;
        return NULL;
    }

    if (syscall_memory_map_map(id, nextAddress))
    {
        lastError = 5;
        return NULL;
    }

    byte* result = (byte*)(nextAddress);
    nextAddress += size;
    return result;
}

bool MemoryMap::unmap(dword id)
{
    if (syscall_memory_map_unmap(id))
    {
        lastError = 6;
        return false;
    }

    return true;
}

dword MemoryMap::getLastError()
{
    return lastError;
}

dword MemoryMap::getSize(dword id)
{
    return syscall_memory_map_get_size(id);
}

}
