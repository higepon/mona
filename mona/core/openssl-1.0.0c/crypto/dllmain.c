#include <stdio.h>

typedef void (FuncVoid)();
extern FuncVoid* __CTOR_LIST__[];
extern FuncVoid* __DTOR_LIST__[];

static int ssl_initialized = 0;

static void invokeFuncList(FuncVoid** list, const char* file, int line)
{
    int count = (int)*list++;
    int i;
    list = (FuncVoid**)((((uint32_t)list) + 3) & ~3);
    if (count == -1)
    {
        for (; *list != NULL; list++) (**list)();
    }
    else
    {
        for (i = 0; i < count; i++, list++)
        {
            (**list)();
        }
    }
}

int dllmain(uint32_t reason)
{
    switch (reason)
    {
    case 0: // DLL_PROCESS_ATTACH
        // Prevent static class initialize invoked twice. (ex) BitMap on MemoryMap.
        if (!ssl_initialized) {
            invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
            ssl_initialized = 1;
        }
        break;
    case 1: // DLL_PROCESS_DETACH
        invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
        break;
    default:
        break;
    }
    return 1;
}

__attribute__((constructor)) void ssl_initialize()
{
    if (ssl_initialized) return;
    ssl_initialized = 1;
}
