#ifndef __MONAPI_IO_H__
#define __MONAPI_IO_H__

#include <sys/types.h>
#include <monapi/messages.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern byte inp8(dword port);
extern void outp8(dword port, byte value);
extern word inp16(dword port);
extern void outp16(dword port, word value);
extern dword inp32(dword port);
extern void outp32(dword port, dword value);

extern void monapi_set_irq(int irq, MONAPI_BOOL enabled, MONAPI_BOOL auto_ir2);
extern MONAPI_BOOL monapi_wait_interrupt(dword ms, byte irq, const char* file = "no file", int line = 0);

#define MONAPI_WAIT_INTERRUPT(ms, irq) monapi_wait_interrupt(ms, irq, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif  // __MONAPI_IO_H__
