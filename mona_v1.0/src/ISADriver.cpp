/* ISADriver.cpp (expr)

    Copyright (c) 2003 .mjt
    All rights reserved.
    License=MIT/X Licnese

*/
#include <BitMap.h>
#include <global.h>
#include <sysresource.h>
#include <driver/ISADriver.h>
#include <info.h>
VirtualConsole* ISADriver::console;
ISADriver::ISADriver(VirtualConsole *con){
  console = con;
  irq_init();
}

sys_irq ISADriver::MapIRQ(BitMap *irqmap){
  return irq_request(irqmap);
}

bool ISADriver::AcquireIRQ(sys_irq irq,IRQHandler ih){
  info(DEV_NOTICE,"ISA:AcqureIRQ(%d)\n",irq);
  return irq_acquire(irq,ih);
}

void ISADriver::EnableIRQ(sys_irq irq){
  irq_enable(irq);
  return;
}

void ISADriver::DisableIRQ(sys_irq irq){
  irq_disable(irq);
  return;
}

bool ISADriver::AcquireIO(word start,word len){return true;};
byte ISADriver::InPort8(word port){
  byte r;
  asm volatile ("inb %%dx, %%al": "=a"(r): "d"(port));
  return r;
}
void ISADriver::OutPort8(word port,byte b){
  asm volatile ("outb %%al, %%dx": :"d" (port), "a" (b));
}
word ISADriver::InPort16(word port){
  word w;
  asm volatile ("in %%dx, %%ax": "=a"(port): "d"(w));
  return w;
}

void ISADriver::OutPort16(word port,word w){
  asm volatile ("out %%ax, %%dx": "=d"(port): "a"(w)); /* ? */
}
