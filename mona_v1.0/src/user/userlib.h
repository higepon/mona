#ifndef _MONA_USERLIB_
#define _MONA_USERLIBL_
#include <types.h>

#define SYSTEM_CALL_PROCESS_SLEEP 5
#define SYSTEM_CALL_HEAVEY        6
#define SYSTEM_CALL_PRINT         7
#define SYSTEM_CALL_KILL          8
#define SYSTEM_CALL_PUT_PIXEL     9
#define SYSTEM_CALL_SEND          10
#define SYSTEM_CALL_RECEIVE       11

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

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
int syscall_put_pixel(int x, int y, char color);
int syscall_send(const char* name, Message* message);
int syscall_receive(Message* message);

void* umalloc(unsigned long size);
void ufree(void * address);

#endif
