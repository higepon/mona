#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmessage.h>
#include <monapi/cmemoryinfo.h>
#include <monapi/MemoryMap.h>

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

monapi_cmemoryinfo* monapi_cmemoryinfo_new()
{
    monapi_cmemoryinfo* this = (monapi_cmemoryinfo*)malloc(sizeof(monapi_cmemoryinfo));
    ASSERT(this);
    this->Handle = 0;
    this->Owner  = THREAD_UNKNOWN;
    this->Size   = 0;
    this->Data   = NULL;
    return this;
}

void monapi_cmemoryinfo_delete(monapi_cmemoryinfo* this)
{
    // monapi_cmemoryinfo_dispose(this);
    free(this);
}

int monapi_cmemoryinfo_create(monapi_cmemoryinfo* this, dword size, int prompt)
{
    this->Handle = monapi_cmemorymap_create(size);
    if (this->Handle == 0)
    {
        if (prompt) printf("ERROR\n");
        printf("%s:%d: MemoryMap create error\n", __FILE__, __LINE__);
        return 0;
    }

    if (!monapi_cmemoryinfo_map(this))
    {
        printf("ERROR\n");
        return 0;
    }

    this->Size  = size;
    this->Owner = syscall_get_tid();
    return 1;
}

int monapi_cmemoryinfo_map(monapi_cmemoryinfo* this)
{
    this->Data = monapi_cmemorymap_map(this->Handle);
    if (this->Data != NULL) return 1;

    printf("%s:%d: map error\n", __FILE__, __LINE__);
    monapi_cmemorymap_unmap(this->Handle);
    this->Handle = 0;
    this->Size   = 0;
    return 0;
}

void monapi_cmemoryinfo_dispose(monapi_cmemoryinfo* this)
{
    monapi_cmemorymap_unmap(this->Handle);
    if (this->Owner != syscall_get_tid())
    {
        monapi_cmessage_send_args(this->Owner, MSG_DISPOSE_HANDLE, this->Handle, 0, 0, NULL);
    }
    this->Handle = 0;
}
