#include <monapi.h>

static int dllmain()
{
    return 1;
}

/*----------------------------------------------------------------------
    invoke constructors and destructors
----------------------------------------------------------------------*/
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

void invokeFuncList(FuncVoid** list)
{
    int count = (int)*list++;
    list = (FuncVoid**)((((dword)list) + 3) & ~3);
    if (count == -1)
    {
        for (; *list != NULL; list++) (**list)();
    }
    else
    {
        for (int i = 0; i < count; i++, list++) (**list)();
    }
}

static FuncVoid** ctor_list = NULL;

extern "C" void setConstructorList(FuncVoid** ctors)
{
        ctor_list = ctors;
}

bool isInDLL(FuncVoid** ctors)
{
        return ctor_list != ctors;
}

/*----------------------------------------------------------------------
    memory management
----------------------------------------------------------------------*/
static MonAPI::MemoryManager um;

extern "C" void monapi_initialize_memory(int memorySize)
{
    um.initialize(0xC0000000, 0xC0000000 + memorySize);
    MonAPI::MemoryMap::initialize();
}

/*----------------------------------------------------------------------
    entry point implementation for application
----------------------------------------------------------------------*/
int user_start_impl(FuncMonaMain* monaMain)
{
    bool dll = isInDLL(__CTOR_LIST__);
    if (dll) invokeFuncList(__CTOR_LIST__);

    List<char*>* arg = new HList<char*>();
    setupArguments(arg);

    int result = (*monaMain)(arg);

    delete arg;
    if (dll) invokeFuncList(__DTOR_LIST__);
    exit(result);
    return 0;
}

extern "C" int user_start_c_impl(FuncMain* main)
{
    bool dll = isInDLL(__CTOR_LIST__);
    if (dll) invokeFuncList(__CTOR_LIST__);

    int argc = syscall_get_arg_count();
    char** _argv = new char*[argc];
    for (int i = 0; i < argc; i++)
    {
        _argv[i] = new char[32];
        if (syscall_get_arg(_argv[i], i) == 1)
        {
            argc = i;
            break;
        }
    }
    char** argv = new char*[argc + 1];
    argv[0] = "dummy";
    for (int i = 0; i < argc; i++)
    {
        argv[argc - i] = _argv[i];
    }
    delete [] _argv;
    argc++;

    int result = (*main)(argc, argv);

    for (int i = 1; i < argc; i++) delete [] argv[i];
    delete [] argv;
    if (dll) invokeFuncList(__DTOR_LIST__);
    exit(result);
    return 0;
}

void setupArguments(List<char*>* arg) {

    char* str;
    HList<char*> tmp;
    int num = syscall_get_arg_count();

    for (int i = 0; i < num; i++) {

        str = (char*)malloc(32);
        if (syscall_get_arg(str, i) == 1) break;
        tmp.add(str);
    }

    for (int i = num - 1; i >= 0; i--) {
        arg->add(tmp[i]);
    }

}

/*----------------------------------------------------------------------
    malloc / free
----------------------------------------------------------------------*/
void* malloc(unsigned long size) {
    return um.allocate(size);
}

void free(void * address) {
    um.free(address);
    return;
}

/*----------------------------------------------------------------------
    operator new/delete
----------------------------------------------------------------------*/
void* operator new(size_t size) {
    return um.allocate(size);
}

void operator delete(void* address) {
    um.free(address);
    return;
}

void* operator new[](size_t size) {
    return um.allocate(size);
}

void operator delete[](void* address) {
    um.free(address);
    return;
}
