#include <monapi.h>

/*----------------------------------------------------------------------
    Static
----------------------------------------------------------------------*/
static MonAPI::MemoryManager um;

/*----------------------------------------------------------------------
    entry point for application
----------------------------------------------------------------------*/
int user_start() {

    int result;
    um.initialize(0xC0000000, 0xC0000000 + 8 * 1024 * 1024);
    List<char*>* arg = new HList<char*>();
    setupArguments(arg);
    MonAPI::MemoryMap::initialize();
    result = MonaMain(arg);
    delete arg;
    exit(result);
    return 0;
}

void setupArguments(List<char*>* arg) {

    char* str;
    int num = syscall_get_arg_count();

    for (int i = 0; i < num; i++) {

        str = (char*)malloc(32);
        if (syscall_get_arg(str, i) == 1) break;
        arg->add(str);
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
