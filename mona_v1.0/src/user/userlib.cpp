#include <userlib.h>
#include <MemoryManager.h>

int sleep(dword tick) {return syscall_sleep(tick);}
int print(const char* msg) {return syscall_print(msg);}
int _put_pixel(int x, int y, char color) {return syscall_put_pixel(x, y, color);}
int kill() {return syscall_kill();}
int exit(int error) {return syscall_kill();}
int _send(const char* name, KMessage* message) {return syscall_send(name, message);}
int _receive(KMessage* message) {return syscall_receive(message);}
int mthread_create(dword f) {return syscall_mthread_create(f);}
int mthread_join(dword id) {return syscall_mthread_join(id);}


int monamain();

static MemoryManager um;

int user_start() {

    int result;
    um.initialize(0xC0000000, 0xC0000000 + 1024 * 1024);
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
                 :"ebx", "esi"
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
                 :"ebx", "esi"
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
                 :"ebx", "esi"
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
                 :"ebx", "esi"
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
                 :"ebx", "esi", "ecx", "edi"
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
                 :"ebx"
                 );

    /* don't come here */
    return result;
}

int syscall_send(const char* name, KMessage* message) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_SEND), "m"(name), "m"(message)
                 :"ebx", "esi", "ecx"
                 );

    return result;
}
int syscall_receive(KMessage* message) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_RECEIVE), "m"(message)
                 : "ebx", "esi"
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
                 :"ebx"
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
                 : "ebx", "esi"
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
                 : "ebx", "esi"
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
                 : "ebx", "esi"
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
                 : "ebx", "esi"
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

extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);


void __cxa_pure_virtual() {

    print("__cxa_pure_virtual called\n");
}


void _pure_virtual() {

    print("_pure_virtual called\n");
}

void __pure_virtual() {

    print("_pure_virtual called\n");
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

void printf(const char *format, ...) {

    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  printInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  print("0x");
                  putInt((int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  putCharacter((char)(int)(*list));
                  ((char*)list) += 1;
                  break;
              case '%':
                  putCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            putCharacter(format[i]);
        }
    }
}

void putInt(size_t n, int base) {

    int    power;
    size_t num = n;
    size_t ch;

    for (power = 0; num != 0; power++) {
        num /= base;
    }

    for (int j = 0; j < 8 - power; j++) {
        putCharacter('0');
    }

    for (int i = power -1; i >= 0; i--) {
        ch = n / _power(base, i);
        n %= _power(base, i);

        if (i == 0 && n > 9) {

            putCharacter('A' + n -10);
        } else if (i == 0) {

            putCharacter('0' + n);
        } else if (ch > 9) {

            putCharacter('A' + ch -10);
        } else {

            putCharacter('0' + ch);
        }
    }
}

void putCharacter(char ch) {
    char* str = " ";
    str[0] = ch;
    print(str);
}

void printInt(int num) {

    char revstr[20];
    char str[20];
    int  i = 0;
    int  j = 0;

    /* negative number */
    if (num < 0) {
        str[0] = '-';
        j++;
        num = num * -1;
    }

    /* number to buffer */
    do {
        revstr[i] = '0' + (int)(num % 10);
        num = num / 10;
        i++;
    } while (num != 0);
    revstr[i] = '\0';

    /* revert */
    for (; i >= 0; j++) {
        str[j] = revstr[i - 1];
        i--;
    }

    /* print */
    print(str);
    return;
}

size_t _power(size_t x, size_t y) {

    size_t result = x;

    for (size_t i = 1; i < y; i++) {
        result *= x;
    }
    return result;
}
