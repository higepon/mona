/* ISADriver.cpp (expr) */
#include <BitMap.h>
#include <global.h>
#include <sysresource.h>
#include <driver/ISADriver.h>
VirtualConsole* ISADriver::console;
ISADriver::ISADriver(VirtualConsole *con){
  console = con;
  irq_init();
}

ISADriver::~ISADriver(){
}

sys_irq ISADriver::MapIRQ(BitMap *irqmap){
  return irq_request(irqmap);
}

bool ISADriver::AcquireIRQ(sys_irq irq,IRQHandler* ih){
  console->printf("ISA:AcqureIRQ(%d)\n",irq);
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

bool ISADriver::AcquireIO(word start,word end){return true;};
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
