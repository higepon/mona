#include <types.h>

#define SYSTEM_CALL_PROCESS_SLEEP 5
#define SYSTEM_CALL_HEAVEY        6

int syscall_sleep(dword tick);
int syscall_heavy();
