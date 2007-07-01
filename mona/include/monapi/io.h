#ifndef _MONAPI_IO_
#define _MONAPI_IO_

#include <sys/types.h>
#include <monapi/messages.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern uint8_t inp8(uint32_t port);
extern void outp8(uint32_t port, uint8_t value);
extern uint16_t inp16(uint32_t port);
extern void outp16(uint32_t port, uint16_t value);
extern uint32_t inp32(uint32_t port);
extern void outp32(uint32_t port, uint32_t value);

extern void monapi_set_irq(int irq, MONAPI_BOOL enabled, MONAPI_BOOL auto_ir2);
extern MONAPI_BOOL monapi_wait_interrupt(uint32_t ms, uint8_t irq, const char* file = "no file", int line = 0);
extern void rdtsc(uint32_t* timeL, uint32_t* timeH);
extern void rdtsc_trace(const char* file, int lineno, const char* func);
#define RDTSC_TRACE() rdtsc_trace(__FILE__, __LINE__, __func__);

#define MONAPI_WAIT_INTERRUPT(ms, irq) monapi_wait_interrupt(ms, irq, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif  // _MONAPI_IO_
