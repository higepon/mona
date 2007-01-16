#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_

#include <sys/types.h>

uint32_t shutdown(uint32_t op, uint32_t device);
uint32_t shutdown_by_apm(uint32_t op, uint32_t device);
uint32_t shutdown_by_reboot();

#endif
