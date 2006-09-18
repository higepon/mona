#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_

#include <sys/types.h>

dword shutdown(dword op, dword device);
dword shutdown_by_apm(dword op, dword device);

#endif
