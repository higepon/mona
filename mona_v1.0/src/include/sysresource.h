/* sysresource.h */
#include <BitMap.h>
#ifndef __EXPR__SYS_RESOURCE_H
#define __EXPR__SYS_RESOURCE_H
typedef int sys_irq;
#define IRQ_INVALID 0

class SysresourceHandler{
  public:
    char *Name;
    dword Version;
};

class IRQHandler{
  public:
    virtual void process(void) = 0;
};

void irq_init(void);
sys_irq irq_request(BitMap* irqmap);
bool irq_acquire(sys_irq irq,IRQHandler* ih);
void irq_free(sys_irq irq);
void irq_enable(sys_irq irq);
void irq_disable(sys_irq irq);

#endif
