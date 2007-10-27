#include <monapi.h>

extern "C" int dllmain(uint32_t reason);
extern "C" __attribute__((constructor)) void libgui_initialize();
extern "C" __attribute__((destructor)) void libgui_finalize();
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

int dllmain(uint32_t reason)
{
    switch (reason)
    {
    case 0: // DLL_PROCESS_ATTACH
//        _logprintf("DLL_PROCESS_ATTACH\n");
        invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
        break;
    case 1: // DLL_PROCESS_DETACH
//        _logprintf("LIBC DLL_PROCESS_DETACH\n");
        invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
        break;
    default:
        _printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
        break;
    }
    return 1;
}

bool libgui_initialized = false;
extern "C" __attribute__((constructor)) void monalibc_initialize();

__attribute__((constructor)) void libgui_initialize()
{
    if (libgui_initialized) return;
    monapi_initialize();
    monalibc_initialize();
    libgui_initialized = true;
}

__attribute__((destructor)) void libgui_finalize()
{
}
