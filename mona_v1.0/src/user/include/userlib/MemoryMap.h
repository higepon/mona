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
    static MemoryMap* getInstance();
    dword create(dword size);
    byte* map(dword id);
    bool unmap(dword id);
    dword getLastError() const;
    dword getSize(dword id) const;

private:
    static const dword START_ADDRESS;
    static const dword MAX_SIZE;
    dword lastError;
    dword nextAddress;
};

#endif
