#include <types.h>

#define SYSTEM_CALL_PROCESS_SLEEP 5
#define SYSTEM_CALL_HEAVEY        6
#define SYSTEM_CALL_PRINT         7
#define SYSTEM_CALL_KILL          8
#define SYSTEM_CALL_PUT_PIXEL     9

#define main() monamain()

extern "C" void main();

int sleep(dword tick);
int heavy();
int print(const char*);
int kill();
int _put_pixel(int x, int y, char color);

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
int syscall_put_pixel(int x, int y, char color);

