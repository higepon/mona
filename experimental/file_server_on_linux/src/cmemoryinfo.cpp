#ifdef ON_LINUX
#include <stdio.h>
#include "cmemoryinfo.h"
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
    return NULL;
}

void monapi_cmemoryinfo_delete(monapi_cmemoryinfo* self)
{
}

int monapi_cmemoryinfo_create(monapi_cmemoryinfo* self, dword size, int prompt)
{
    return 1;
}

int monapi_cmemoryinfo_map(monapi_cmemoryinfo* self)
{
    return 0;
}

void monapi_cmemoryinfo_dispose(monapi_cmemoryinfo* self)
{
}
