#include <userlib.h>
#include <MemoryManager.h>

int sleep(dword tick) {return syscall_sleep(tick);}
int print(const char* msg) {return syscall_print(msg);}
int _put_pixel(int x, int y, char color) {return syscall_put_pixel(x, y, color);}
int kill() {return syscall_kill();}
int exit(int error) {return syscall_kill();}
int mthread_create(dword f) {return syscall_mthread_create(f);}
int mthread_join(dword id) {return syscall_mthread_join(id);}


int monamain();

static MemoryManager um;

int user_start() {

    int result;
    um.initialize(0xC0000000, 0xC0000000 + 1024 * 1024);
    result = monamain();
    //    exit(result);
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

int syscall_load_process(const char* name) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_LOAD_PROCESS), "m"(name)
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

int syscall_send(dword pid, MessageInfo* message) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_SEND), "m"(pid), "m"(message)
                 :"ebx", "esi", "ecx"
                 );

    return result;
}
int syscall_receive(MessageInfo* message) {

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

dword syscall_lookup(const char* name) {

    dword pid;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(pid)
                 :"g"(SYSTEM_CALL_LOOKUP), "m"(name)
                 : "ebx", "esi"
                 );

    return pid;
}

int syscall_get_vram_info(ScreenInfo* info) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_GET_VRAM_INFO), "m"(info)
                 : "ebx", "esi"
                 );

    return result;
}

int syscall_map(dword pid, dword sharedId, dword linearAddress, dword size) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "movl %5  , %%edx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_MAP), "m"(pid), "m"(sharedId), "m"(linearAddress), "m"(size)
                 : "ebx", "esi", "ecx", "edi", "edx"
                 );

    return result;
}

int syscall_get_cursor(int* x, int* y) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_GET_CURSOR), "m"(x), "m"(y)
                 : "ebx", "esi", "ecx"
                 );

    return result;
}

int syscall_set_cursor(int x, int y) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_SET_CURSOR), "m"(x), "m"(y)
                 : "ebx", "esi", "ecx"
                 );

    return result;
}

int syscall_file_open(char* path, char* file, dword* size) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_FILE_OPEN), "m"(path), "m"(file), "m"(size)
                 : "ebx", "esi", "ecx", "edi"
                 );

    return result;
}

int syscall_file_read(char* buf, dword size, dword* readSize) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_FILE_READ), "m"(buf), "m"(size), "m"(readSize)
                 : "ebx", "esi", "ecx", "edi"
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
    I/O
----------------------------------------------------------------------*/
byte inportb(dword port) {

    byte ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
    return ret;
}

void outportb(dword port, byte value) {
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
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

/*----------------------------------------------------------------------
    Screen
----------------------------------------------------------------------*/
Screen::Screen() {

    /* get and set vram information */
    syscall_get_vram_info(&sinfo);
    vram_        = (byte*)sinfo.vram;
    bpp_         = sinfo.bpp;
    xResolution_ = sinfo.x;
    yResolution_ = sinfo.y;
}

Screen::~Screen() {
}

void Screen::putPixel16(int x, int y, dword color) {

    int bytesPerPixel = bpp_ / 8;
    byte* vram       = vram_;

    vram += (x + y * xResolution_) * bytesPerPixel;
    *((word*)vram) = (word)color;
}

void Screen::fillRect16(int x, int y, int w, int h, dword color) {

        dword bytesPerPixel = bpp_ / 8;

        byte* position = vram_ + (x + y * xResolution_) * bytesPerPixel;
        byte* temp     = position;

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                *((word*)temp) = color;
                temp += bytesPerPixel;
            }
            position += xResolution_ * bytesPerPixel;
            temp = position;
        }
}

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
int Message::send(dword pid, MessageInfo* info) {
    return syscall_send(pid, info);
}

int Message::receive(MessageInfo* info) {
    return syscall_receive(info);
}

dword Message::lookup(const char* name) {
    return syscall_lookup(name);
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
                  putInt((int)*list, 10);
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

    static char buf[256];

    int geta = 8;
    int num  = n;
    if (base == 10 && num != 0) {
        for (geta = 0; num; num /= 10, geta++);
    } else if (base == 10 && num == 0) {
        geta = 1;
    }

    char* p = ltona(n, buf, geta, base);

    for (; *p != '\0'; p++) {
        putCharacter(*p);
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
