#include <monapi.h>

extern MonAPI::Stream* inStream;
extern MonAPI::Stream* outStream;
extern "C" void init_stdio(void);

extern "C" int dllmain(uint32_t reason);
extern "C" __attribute__((constructor)) void monalibc_initialize();
extern "C" __attribute__((destructor)) void monalibc_finalize();
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

int dllmain(uint32_t reason)
{
    switch (reason)
    {
    case 0: // DLL_PROCESS_ATTACH

        _logprintf("DLL_PROCESS_ATTACH\n");
        invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
        break;
    case 1: // DLL_PROCESS_DETACH
        _logprintf("LIBC DLL_PROCESS_DETACH\n");
        invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
        break;
    default:
        _printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
        break;
    }
    return 1;
}

bool monalibc_initialized = false;

__attribute__((constructor)) void monalibc_initialize()
{
    if (monalibc_initialized) return;
    monalibc_initialized = true;
    monapi_initialize();

// known bug.
#if 0
    uint32_t handle;
    handle = MonAPI::System::getProcessStdinID();
    _logprintf("%s:%d stdin handle=%x\n", __FILE__, __LINE__, handle);
    inStream = MonAPI::Stream::FromHandle(handle);
    handle = MonAPI::System::getProcessStdoutID();
    _logprintf("%s:%d stdout handle=%x\n", __FILE__, __LINE__, handle);
    outStream = MonAPI::Stream::FromHandle(handle);
#endif
    init_stdio();
}

__attribute__((destructor)) void monalibc_finalize()
{
}

// if __attribute__((constructor)) doesn't work, use this.
//static struct MonalibcInitWrapper {MonalibcInitWrapper(){monalibc_initialize();}} Monalibc_initializer;
