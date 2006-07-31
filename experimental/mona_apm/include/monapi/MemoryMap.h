/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#ifndef _MONA_USERLIB_MEMORY_MAP_
#define _MONA_USERLIB_MEMORY_MAP_

#include <sys/types.h>

#ifdef __cplusplus
namespace MonAPI
{
    class MemoryMap
    {
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

}

extern "C"
{
#endif
extern dword monapi_cmemorymap_create(dword size);
extern byte* monapi_cmemorymap_map(dword id);
extern int monapi_cmemorymap_unmap(dword id);
#ifdef __cplusplus
}
#endif

#endif
