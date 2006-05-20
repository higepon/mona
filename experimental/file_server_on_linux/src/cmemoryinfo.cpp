#ifdef ON_LINUX
#include <stdio.h>
#include <stdlib.h>
#include "cmemoryinfo.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#else
#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmemoryinfo.h>
#include <monapi/Message.h>
#include <monapi/MemoryMap.h>
#endif

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

using namespace MonAPI;

monapi_cmemoryinfo* monapi_cmemoryinfo_new()
{
    monapi_cmemoryinfo* self = (monapi_cmemoryinfo*)malloc(sizeof(monapi_cmemoryinfo));
    ASSERT(self);
    self->Handle = 0;
    self->Owner  = THREAD_UNKNOWN;
    self->Size   = 0;
    self->Data   = NULL;
    return self;
}

void monapi_cmemoryinfo_delete(monapi_cmemoryinfo* self)
{
    free(self);
}

int monapi_cmemoryinfo_create(monapi_cmemoryinfo* self, dword size, int prompt)
{
#ifdef ON_LINUX
    if (-1 == (self->Handle = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666)))
    {
        perror("shmget");
        exit(-1);
    }

    if (!monapi_cmemoryinfo_map(self))
    {
        if (prompt) printf("ERROR\n");
        printf("%s:%d: MemoryMap map error\n", __FILE__, __LINE__);
        return 0;
    }
    self->Size  = size;
    self->Owner = getpid();
    return 1;

#else
    self->Handle = monapi_cmemorymap_create(size);
    if (self->Handle == 0)
    {
        if (prompt) printf("ERROR\n");
        printf("%s:%d: MemoryMap create error\n", __FILE__, __LINE__);
        return 0;
    }

    if (!monapi_cmemoryinfo_map(self))
    {
        if (prompt) printf("ERROR\n");
        printf("%s:%d: MemoryMap map error\n", __FILE__, __LINE__);
        return 0;
    }

    self->Size  = size;
    self->Owner = syscall_get_tid();
    return 1;
#endif
}

int monapi_cmemoryinfo_map(monapi_cmemoryinfo* self)
{
#ifdef ON_LINUX
    self->Data = (byte*)shmat(self->Handle, 0, 0);
    if (-1 == (int)self->Data)
    {
        perror("shmat");
        exit(-1);
    }
    return 1;
#else
    self->Data = monapi_cmemorymap_map(self->Handle);
    if (self->Data != NULL) return 1;

    printf("%s:%d: map error\n", __FILE__, __LINE__);
    monapi_cmemorymap_unmap(self->Handle);
    self->Handle = 0;
    self->Size   = 0;
    return 0;
#endif
}

void monapi_cmemoryinfo_dispose(monapi_cmemoryinfo* self)
{
#ifdef ON_LINUX
    shmdt(self->Data);
    self->Handle = 0;
#else
    monapi_cmemorymap_unmap(self->Handle);
    if (self->Owner != syscall_get_tid())
    {
        Message::send(self->Owner, MSG_DISPOSE_HANDLE, self->Handle);
    }
    self->Handle = 0;
#endif
}
