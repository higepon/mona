#ifndef _MONA_USERLIB_MEMORY_MAP_
#define _MONA_USERLIB_MEMORY_MAP_

#include <types.h>
#include <Map.h>
#include <HList.h>

extern "C" int syscall_map2(MappingInfo* info);
extern "C" int syscall_unmap2(dword id);

/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/
class MemoryMap
{
private:
    MemoryMap();
    virtual ~MemoryMap();

public:
    static void initialize();
    static dword create(dword size);
    static byte* map(dword id);
    static bool unmap(dword id);
    static dword getLastError();
    static dword getSize(dword id);

private:
    static const dword START_ADDRESS;
    static const dword MAX_SIZE;
    static dword lastError;
    static dword nextAddress;
};

#endif
