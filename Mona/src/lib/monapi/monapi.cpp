#include <sys/List.h>
#include <monapi/syscall.h>

extern void monapi_initialize_memory();
extern int user_start_impl(FuncMonaMain* monaMain);

extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

/*----------------------------------------------------------------------
    entry point for application
----------------------------------------------------------------------*/
extern "C" int user_start()
{
    monapi_initialize_memory();
    setConstructorList(__CTOR_LIST__);
    invokeFuncList(__CTOR_LIST__);
    int result = user_start_impl(MonaMain);
    invokeFuncList(__DTOR_LIST__);
    return result;
}
