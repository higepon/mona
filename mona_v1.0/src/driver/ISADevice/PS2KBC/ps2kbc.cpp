/* ps2kbc */
#include "ps2kbc.h" //That's all you need
#include <io.h>

//#include <global.h>
#define PS2_CMD 0x64
#define PS2_DATA 0x60

byte PS2KBC::OutBuf[32];
byte PS2KBC::InBuf[32];
byte PS2KBC::InBufEndPtr;
byte PS2KBC::OutBufEndPtr;

ISADriver* PS2KBC::isa_;

PS2KBC *ps2;

Globals *g;

void keyboardirqhandler(void){
      g->debug->printf("PS2:INT Key\n");
      ps2->Recv();
}

void mouseirqhandler(void){
      g->debug->printf("PS2:INT Mouse\n");
      ps2->Recv();
}

byte inportb_(dword port) {

    byte ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
    return ret;
}

void outportb_(dword port, byte value) {
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

bool PS2KBC::Recv(void){
    g->debug->printf("PS2:Recv...\n ");
    if(!(inportb_(PS2_CMD) & 1)){
      g->debug->printf("(no data)\n");
      return false;
    }
    while(inportb_(PS2_CMD) & 1){
      InBuf[InBufEndPtr] = inportb_(PS2_DATA);
      g->debug->printf("%x ",InBuf[InBufEndPtr]);
      InBufEndPtr++;
    }
    g->debug->printf("\n");
    return true;
}


bool PS2KBC::Send(void){
  byte c;
  g->debug->printf("PS2:Send:Size = %d\n ",OutBufEndPtr);
  if(OutBufEndPtr){
    
    for(c=0;c!=OutBufEndPtr;c++){
      g->debug->printf("%x ",OutBuf[c]);
    }
    g->debug->printf("\n");
    for(c=0;c!=OutBufEndPtr;c++){
      while(inportb_(PS2_CMD) & 3);
      if(inportb_(PS2_CMD) & 0xe0){
        g->debug->printf("PS2:Write Error\n");
        return false;
      }
      if(c){
        outportb_(PS2_DATA,OutBuf[c]);
      }else{
        outportb_(PS2_CMD,OutBuf[c]);
      }
    }
    while(inportb_(PS2_CMD) & 2);
    Recv();
  }
  return true;
}

PS2KBC::PS2KBC(ISADriver *isa,Globals* G){
  char c;
  
	g = G;
	isa_ = isa;
  
	g->debug->printf("PS2:init...\n");
  isa_->AcquireIO(PS2_CMD,1);
  isa_->AcquireIO(PS2_DATA,1);
  
  ps2 = this; /* temp */
  
  InBufEndPtr = 0;
  OutBufEndPtr = 0;
  OutBuf[0] = 0x20;
  OutBufEndPtr = 1;
  if(!Send()) return;
  if(InBufEndPtr != 1){
    g->debug->printf("PS2:init: cmd register value error.\n");
  }
  InBufEndPtr = 0;
  c = InBuf[0];
  c |= 0x3; /* INT Enable both */
  OutBuf[0] = 0x60;
  OutBuf[1] = c;
  OutBufEndPtr = 2;
  Send();
  
  /* KB init */
  g->debug->printf("ps2:installing Keyboard IRQ Handler(%x).\n",(dword)keyboardirqhandler);
  
  if(!isa_->AcquireIRQ(1, keyboardirqhandler)){
    g->debug->printf("PS2:init:can't acquire IRQ.\n");
    return;
  }
  isa_->EnableIRQ(1);
  
	return;
	
  /* MOUSE init */
  g->debug->printf("ps2:checking Mouse...");
  
  OutBuf[0] = 0xa9;
  OutBuf[1] = 0xff;
  OutBufEndPtr = 2;
  Send();
  InBufEndPtr = 0;
  if(InBuf[0] != 0){
    g->debug->printf("error(%x).\n",InBuf[0]);
    return;
  }
  g->debug->printf("ok.\n");
  OutBuf[0] = 0xa8;
  OutBufEndPtr = 1;
  Send();
  
  OutBuf[0] = 0xd4;
  OutBuf[1] = 0xff;
  OutBufEndPtr = 2;
  Send();
  
  g->debug->printf("ps2:installing Mouse IRQ Handler(%x).\n",(dword)mouseirqhandler);
  if(!isa_->AcquireIRQ(12, keyboardirqhandler)){
    g->debug->printf("PS2:init:can't acquire IRQ.\n");
    return;
  }
  isa_->EnableIRQ(12);
  
  InBufEndPtr = 0;
}

