/* Mouse.cpp (expr) */
#include <driver/ISADriver.h>
#include <io.h>
#include "Mouse.h"

ISADriver* bus;
class MouseIRQHandler : public IRQHandler {
  public:
    void process(void){
      bus->console->printf("MouseInterrupt:\n");
    }
};
MouseIRQHandler mh;
dword Mouse::x;
dword Mouse::y;
dword Mouse::z;
dword Mouse::btncount[3];
bool Mouse::btnstate[3];
ISADriver* Mouse::isa_;
Mouse::Mouse(ISADriver* isa){
  isa_ = isa;
  bus = isa;
  if(!isa_->AcquireIRQ(12,&mh)){
    isa_->console->printf("Mouse:can't acquire IRQ.\n");
  }
  isa_->console->printf("Mouse:Installed IRQ Handler.\n");
  isa_->EnableIRQ(12);
  /* init */
  
  /* ~init */
  Send(0xff); /* reset */  
  Send(0xf4); /* enable */
}

void Mouse::Send(byte b){
  outportb(0x64,0xd4);
  while(inportb(0x64) & 0x01);
  outportb(0x60,b);
/*
  isa_->OutPort8(0x64,0xd4);
  isa_->OutPort8(0x60,b);
*/
}

byte Mouse::Recv(){
  return 0;
}





