#ifndef _MONA_USERLIB_
#define _MONA_USERLIB_
#include <types.h>
#include <string.h>

#define SYSTEM_CALL_PROCESS_SLEEP  5
#define SYSTEM_CALL_HEAVEY         6
#define SYSTEM_CALL_PRINT          7
#define SYSTEM_CALL_KILL           8
#define SYSTEM_CALL_PUT_PIXEL      9
#define SYSTEM_CALL_SEND           10
#define SYSTEM_CALL_RECEIVE        11
#define SYSTEM_CALL_MTHREAD_CREATE 12
#define SYSTEM_CALL_MTHREAD_JOIN   13
#define SYSTEM_CALL_MUTEX_CREATE   14
#define SYSTEM_CALL_MUTEX_LOCK     15
#define SYSTEM_CALL_MUTEX_TRYLOCK  16
#define SYSTEM_CALL_MUTEX_UNLOCK   17
#define SYSTEM_CALL_MUTEX_DESTROY  18
#define SYSTEM_CALL_LOOKUP         19

#define main() monamain()

extern "C" int user_start();

int sleep(dword tick);
int heavy();
int print(const char*);
int kill();
int exit(int error);
int _put_pixel(int x, int y, char color);
int mthread_create(dword f);
int mthread_join(dword id);

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
int syscall_put_pixel(int x, int y, char color);
int syscall_send(dword pid, MessageInfo* message);
int syscall_receive(MessageInfo* message);
int syscall_mthread_create(dword f);
int syscall_mthread_join(dword id);
int syscall_mutex_create();
int syscall_mutex_trylock();
int syscall_mutex_lock();
int syscall_mutex_unlock();
dword syscall_lookup(const char* name);
int syscall_mutex_destroy(int id);
void* malloc(unsigned long size);
void free(void * address);

void* operator new(size_t size);
void  operator delete(void* address);

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex {

  public:
    Mutex();
    ~Mutex();
    int init();
    int lock();
    int unlock();
    int tryLock();
    int destory();

  public:
    inline int getId() const {
        return mutexId_;
    }

  private:
    int mutexId_;
};

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {

  private:
    Message();
    virtual ~Message();

  public:
    static int send(dword pid, MessageInfo* info);
    static int receive(MessageInfo* info);
    static dword lookup(const char* name);
};


void putCharacter(char ch);
void putInt(size_t n, int base);
void printf(const char *format, ...);
void printInt(int num);
size_t _power(size_t x, size_t y);
#endif
