/* ps2kbc */
#include "ps2kbc.h" //That's all you need
#include <io.h>

#include <global.h>

#define PS2_CMD 0x64
#define PS2_DATA 0x60

byte PS2KBC::OutBuf[32];
byte PS2KBC::InBuf[32];
byte PS2KBC::InBufEndPtr;
byte PS2KBC::OutBufEndPtr;

ISADriver* PS2KBC::isa_;

PS2KBC *ps2;
/*
class KeyBoardIRQHandler : public IRQHandler {
  public:
    virtual void process(void){
      g_console->printf("PS2:INT\n");
      g_demo_step++;
      ps2->Recv();
    }
};
*/

void keyboardirqhandler(void){
      g_console->printf("PS2:INT Key\n");
      ps2->Recv();
}

void mouseirqhandler(void){
      g_console->printf("PS2:INT Mouse\n");
      ps2->Recv();
}

bool PS2KBC::Recv(void){
    isa_->console->printf("PS2:Recv...\n ");
    if(!(inportb(PS2_CMD) & 1)){
      isa_->console->printf("(no data)\n");
      return false;
    }
    while(inportb(PS2_CMD) & 1){
      InBuf[InBufEndPtr] = inportb(PS2_DATA);
      isa_->console->printf("%x ",InBuf[InBufEndPtr]);
      InBufEndPtr++;
    }
    isa_->console->printf("\n");
    return true;
}


bool PS2KBC::Send(void){
  byte c;
  isa_->console->printf("PS2:Send:Size = %d\n ",OutBufEndPtr);
  if(OutBufEndPtr){
    
    for(c=0;c!=OutBufEndPtr;c++){
      isa_->console->printf("%x ",OutBuf[c]);
    }
    isa_->console->printf("\n");
    for(c=0;c!=OutBufEndPtr;c++){
      while(inportb(PS2_CMD) & 3);
      if(inportb(PS2_CMD) & 0xe0){
        isa_->console->printf("PS2:Write Error\n");
        return false;
      }
      if(c){
        outportb(PS2_DATA,OutBuf[c]);
      }else{
        outportb(PS2_CMD,OutBuf[c]);
      }
    }
    while(inportb(PS2_CMD) & 2);
    Recv();
  }
  return true;
}

PS2KBC::PS2KBC(ISADriver *isa){
  char c;
  isa_->console->printf("PS2:init...\n");
  isa_->AcquireIO(PS2_CMD,1);
  isa_->AcquireIO(PS2_DATA,1);
  isa_ = isa;
  
  ps2 = this; /* temp */
  
  InBufEndPtr = 0;
  OutBufEndPtr = 0;
  OutBuf[0] = 0x20;
  OutBufEndPtr = 1;
  if(!Send()) return;
  if(InBufEndPtr != 1){
    isa_->console->printf("PS2:init: cmd register value error.\n");
  }
  InBufEndPtr = 0;
  c = InBuf[0];
  c |= 0x3; /* INT Enable both */
  OutBuf[0] = 0x60;
  OutBuf[1] = c;
  OutBufEndPtr = 2;
  Send();
  
  /* KB init */
  g_console->printf("ps2:installing Keyboard IRQ Handler(%x).\n",(dword)keyboardirqhandler);
  
  if(!isa_->AcquireIRQ(1, keyboardirqhandler)){
    isa_->console->printf("PS2:init:can't acquire IRQ.\n");
    return;
  }
  isa_->EnableIRQ(1);
  
  /* MOUSE init */
  g_console->printf("ps2:checking Mouse...");
  
  OutBuf[0] = 0xa9;
  OutBuf[1] = 0xff;
  OutBufEndPtr = 2;
  Send();
  InBufEndPtr = 0;
  if(InBuf[0] != 0){
    g_console->printf("error(%x).\n",InBuf[0]);
    return;
  }
  g_console->printf("ok.\n");
  OutBuf[0] = 0xa8;
  OutBufEndPtr = 1;
  Send();
  
  OutBuf[0] = 0xd4;
  OutBuf[1] = 0xff;
  OutBufEndPtr = 2;
  Send();
  
  g_console->printf("ps2:installing Mouse IRQ Handler(%x).\n",(dword)mouseirqhandler);
  if(!isa_->AcquireIRQ(12, keyboardirqhandler)){
    isa_->console->printf("PS2:init:can't acquire IRQ.\n");
    return;
  }
  isa_->EnableIRQ(12);
  
  InBufEndPtr = 0;
}

