#include <userlib.h>
#include <MemoryManager.h>

int sleep(dword tick) {return syscall_sleep(tick);}
int print(const char* msg) {return syscall_print(msg);}
int _put_pixel(int x, int y, char color) {return syscall_put_pixel(x, y, color);}
int kill() {return syscall_kill();}
int exit(int error) {return syscall_kill();}
int _send(const char* name, Message* message) {return syscall_send(name, message);}
int _receive(Message* message) {return syscall_receive(message);}
int mthread_create(dword f) {return syscall_mthread_create(f);}
int mthread_join(dword id) {return syscall_mthread_join(id);}


int monamain();

static MemoryManager um;

int user_start() {
    int result;
    um.initialize(0xC0000000,0xC0000000 + 1024 * 1024);
    result = monamain();
    exit(result);
    for (;;);
}

int syscall_mthread_create(dword f) {

    int result;

    asm volatile("movl $%c2, %%ebx \n"
                 "movl %1  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"m"(f), "g"(SYSTEM_CALL_MTHREAD_CREATE)
                 );

    return result;
}

int syscall_mthread_join(dword id) {

    int result;

    asm volatile("movl $%c2, %%ebx \n"
                 "movl %1  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"m"(id), "g"(SYSTEM_CALL_MTHREAD_JOIN)
                 );

    return result;
}


int syscall_sleep(dword tick) {

    int result;

    asm volatile("movl $%c2, %%ebx \n"
                 "movl %1  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"m"(tick), "g"(SYSTEM_CALL_PROCESS_SLEEP)
                 );

    return result;
}

int syscall_print(const char* msg) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_PRINT), "m"(msg)
                 );

    return result;
}

int syscall_put_pixel(int x, int y, char color) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_PUT_PIXEL), "m"(x), "m"(y), "m"(color)
                 );

    return result;
}

int syscall_kill() {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_KILL)
                 );

    /* don't come here */
    return result;
}

int syscall_send(const char* name, Message* message) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_SEND), "m"(name), "m"(message)
                 );

    return result;
}
int syscall_receive(Message* message) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_RECEIVE), "m"(message)
                 );

    return result;
}

int syscall_mutex_create() {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MUTEX_CREATE)
                 );

    return result;
}

int syscall_mutex_trylock(int id) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MUTEX_TRYLOCK), "m"(id)
                 );

    return result;
}

int syscall_mutex_lock (int id ) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MUTEX_LOCK), "m"(id)
                 );

    return result;
}

int syscall_mutex_unlock(int id) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MUTEX_UNLOCK), "m"(id)
                 );

    return result;
}

int syscall_mutex_destroy(int id) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MUTEX_DESTROY), "m"(id)
                 );

    return result;
}


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

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
Mutex::Mutex() {
}

Mutex::~Mutex() {
}

int Mutex::init() {
    mutexId_ = syscall_mutex_create();
    return mutexId_;
}

int Mutex::lock() {
    return syscall_mutex_lock(mutexId_);
}

int Mutex::unlock() {
    return syscall_mutex_unlock(mutexId_);
}

int Mutex::tryLock() {
    return syscall_mutex_trylock(mutexId_);
}

int Mutex::destory() {
    return syscall_mutex_destroy(mutexId_);
}
