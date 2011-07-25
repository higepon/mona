#include <monapi/syscall.h>
#include <monapi.h>

extern "C" void monapi_initialize_memory(int memorySize);
extern "C" int main(int argc, char** argv);
extern "C" int user_start_c_impl(FuncMain* main);

extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

// warning!
// don't put functions above user_start.
/*----------------------------------------------------------------------
    entry point for application
----------------------------------------------------------------------*/
extern "C" int user_start()
{
  monapi_initialize_memory(PROCESS_HEAP_SIZE);
  invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
  setConstructorList(__CTOR_LIST__);

  int result = user_start_c_impl(main);
  invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
  exit(result);
  return result;
}
