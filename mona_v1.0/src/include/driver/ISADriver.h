/* ISADriver.h */
#ifndef __EXPR__ISADRIVER_H
#define __EXPR__ISADRIVER_H
#include <global.h>
#include <BitMap.h>
#include <sysresource.h>
class ISADriver : public SysresourceHandler{
  public:
    sys_irq MapIRQ(BitMap *irqm);
    bool AcquireIRQ(sys_irq irq,IRQHandler *irqh);
    //bool PrepareWaitForIRQ(sys_irq irq);
    //bool WaitForIRQWithTimeout(dword timeout);
    bool AcquireIO(word start,word end);
    byte InPort8(word port);
    void OutPort8(word port,byte b);
    word InPort16(word port);
    void OutPort16(word port,word b);
    //AcquireDMA(sys_dma dma);
    //AcquireMemoryWindow(dword start,dword end);
    //AcquirePageWindow(dword start,dword end);
};
#endif // __EXPR__ISADRIVER_H
