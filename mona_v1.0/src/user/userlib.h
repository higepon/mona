#include <types.h>

#define SYSTEM_CALL_PROCESS_SLEEP 5
#define SYSTEM_CALL_HEAVEY        6
#define SYSTEM_CALL_PRINT         7
#define SYSTEM_CALL_KILL          8

#define main() monamain()

extern "C" void main();

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
