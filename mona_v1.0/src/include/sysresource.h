/* sysresource.h */
#include <BitMap.h>
#ifndef __EXPR__SYS_RESOURCE_H
#define __EXPR__SYS_RESOURCE_H
#define sys_irq int
#define IRQ_INVALID 0
class IRQHandler{
  public:
    IRQHandler(sys_irq irq);
    ~IRQHandler();
    virtual void process();
    static sys_irq irq;
};
sys_irq irq_request(BitMap* irqmap);
bool irq_acquire(IRQHandler *ih);
void irq_free(sys_irq irq);
void irq_enable(sys_irq irq);
void irq_disable(sys_irq irq);
#endif
