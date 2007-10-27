/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/

#ifndef _MONAPI_MEMORY_MAP_
#define _MONAPI_MEMORY_MAP_

#include <sys/types.h>
#include <monapi/Mutex.h>

#ifdef __cplusplus
namespace MonAPI
{
    class MemoryMap
    {
    public:
        static void initialize();
        static uint32_t create(uint32_t size);
        static uint8_t* map(uint32_t id);
        static bool unmap(uint32_t id);
        static uint32_t getLastError();
        static uint32_t getSize(uint32_t id);
        static const char* getLastErrorString();
        enum
        {
            ERROR_SIZE_ZERO,
            ERROR_NOT_ENOUGH_KERNEL_MEMORY,
            ERROR_MEMORY_MAP_NOT_FOUND,
            ERROR_NOT_ENOUGH_ADDRESS_SPACE,
            ERROR_NOT_MAP_ATATCH_ERROR

        };
    private:
        static const uint32_t START_ADDRESS;
        static const uint32_t MAX_SIZE;
        static uint32_t lastError;
        static uint32_t nextAddress;
        static Mutex mutex;
    };

}

extern "C"
{
#endif
extern uint32_t monapi_cmemorymap_create(uint32_t size);
extern uint8_t* monapi_cmemorymap_map(uint32_t id);
extern int monapi_cmemorymap_unmap(uint32_t id);
#ifdef __cplusplus
}
#endif

#endif
