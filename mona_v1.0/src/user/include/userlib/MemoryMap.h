#ifndef _MONA_USERLIB_MEMORY_MAP_
#define _MONA_USERLIB_MEMORY_MAP_

#include <types.h>
#include <Map.h>

extern "C" int syscall_map2(MappingInfo* info);
extern "C" int syscall_unmap2(dword id);
/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/
class MemoryMap {

  private:
    MemoryMap(){};
    virtual ~MemoryMap(){};

    public:
    inline static MemoryMap* create() {
        static MemoryMap map;
        return &map;
    }

    inline dword map(dword pid, dword linearAddress, dword linearAddress2, dword size) {

        if (linearAddress < 0x90000000 || linearAddress >=0xA0000000) {
            info_.errorCd = 4;
            return 0;
        }

        if (linearAddress2 < 0x90000000 || linearAddress2 >=0xA0000000) {
            info_.errorCd = 4;
            return 0;
        }

        info_.attachPid = pid;
        info_.linearAddress1 = linearAddress;
        info_.linearAddress2 = linearAddress2;
        info_.size           = size;

        return syscall_map2(&info_);
    }

    inline int unmap(dword sharedId) {
        return syscall_unmap2(sharedId);
    }

    inline dword getLastErrorCode() const {
        return info_.errorCd;
    }

  private:
    MappingInfo info_;
};

#endif
