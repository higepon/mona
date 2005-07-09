#include "time.h"
#include <monapi/syscall.h>

time_t time(time_t *t)
{
	return syscall_get_tick();
}
