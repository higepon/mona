/* sysresource.cpp */
#include <sysresource.h>
#include <global.h>
sys_irq irq_request(BitMap* irqmap){
  return 0;
}

bool irq_acquire(IRQHandler ih){
  return false;
}

void irq_free(sys_irq irq){
  return;
}

void irq_enable(sys_irq irq){
}
void irq_disable(sys_irq irq){
}

