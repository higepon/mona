/* ISADriver.h */
#ifndef __EXPR__ISADRIVER_H
#define __EXPR__ISADRIVER_H
#include <types.h>
#include <BitMap.h>
#include <sysresource.h>
#include <GraphicalConsole.h>
class ISADriver : public SysresourceHandler{
  public:
    ISADriver(VirtualConsole *con);
    //~ISADriver(); /* Kernel Driver is NEVER destructs */
    virtual sys_irq MapIRQ(BitMap *irqm);
    virtual bool AcquireIRQ(sys_irq irq,IRQHandler irqh);
    virtual void EnableIRQ(sys_irq irq);
    virtual void DisableIRQ(sys_irq irq);
    
    //bool PrepareWaitForIRQ(sys_irq irq);
    //bool WaitForIRQWithTimeout(dword timeout);
    virtual bool AcquireIO(word start,word end);

    byte InPort8(word port);
    void OutPort8(word port,byte b);
    word InPort16(word port);
    void OutPort16(word port,word b);
    
    //AcquireDMA(sys_dma dma);
    //AcquireMemoryWindow(dword start,dword end);
    //AcquirePageWindow(dword start,dword end);
    
  public:
    static VirtualConsole *console;
    
};
#endif // __EXPR__ISADRIVER_H
