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

#define main() monamain()

extern "C" int user_start();

int sleep(dword tick);
int heavy();
int print(const char*);
int kill();
int exit(int error);
int _send(const char* name, Message* message);
int _receive(Message* message);
int _put_pixel(int x, int y, char color);
int mthread_create(dword f);
int mthread_join(dword id);

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
int syscall_put_pixel(int x, int y, char color);
int syscall_send(const char* name, Message* message);
int syscall_receive(Message* message);
int syscall_mthread_create(dword f);
int syscall_mthread_join(dword id);
void* malloc(unsigned long size);
void free(void * address);

void* operator new(size_t size);
void  operator delete(void* address);

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
typedef struct StMutex {
    dword dummy;
};

class Mutex {

  public:
    Mutex();
    ~Mutex();

  public:
    inline StMutex* getStMutex() const {
        return mutex;
    }

  private:
    StMutex* mutex;
};


#endif
