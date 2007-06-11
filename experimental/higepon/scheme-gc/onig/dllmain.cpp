#include <monapi.h>

extern "C" int dllmain(uint32_t reason);
extern "C" __attribute__((constructor)) void libonig_initialize();
extern "C" __attribute__((destructor)) void libonig_finalize();
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

int dllmain(uint32_t reason)
{
    switch (reason)
    {
    case 0: // DLL_PROCESS_ATTACH
        invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
        break;
    case 1: // DLL_PROCESS_DETACH
        invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
        break;
    default:
        break;
    }
    return 1;
}

bool libonig_initialized = false;
extern "C" __attribute__((constructor)) void monalibc_initialize();

__attribute__((constructor)) void libonig_initialize()
{
    if (libonig_initialized) return;
    monapi_initialize();
    monalibc_initialize();
    libonig_initialized = true;
}

__attribute__((destructor)) void libonig_finalize()
{
}
