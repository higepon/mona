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

bool Mouse::PS2Wait(void){
  isa_->console->printf("Mouse:wait...\n");
  while(isa_->InPort8(0x64) & 8);
  return true;
}

void Mouse::PS2Comm(byte *b,int sendsize,int recvsize){
  int i;
  if(sendsize){
    for(i = 0;i!=size;i++){
      PS2Wait(); 
      if(!i){
        isa_->console->printf("Mouse:PS2Send KBC %x\n",b[i]);isa_->OutPort8(0x64,b[i]);
      }else{
        isa_->console->printf("Mouse:PS2Send KBC %x\n",b[i]);isa_->OutPort8(0x60,b[i]);
      }
    }
  }
  if(recvsize){
    
  }
}

void Mouse::Send(byte b){
  
}

byte Mouse::Recv(){
  return 0;
}





