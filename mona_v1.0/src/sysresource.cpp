/* sysresource.cpp (expr) */
#include <sysresource.h>
#include <global.h>
#include <pic.h>
#include<idt.h> /* —v‚ç‚È‚¢? */

/*
class NullIRQHandler : public IRQHandler {
  public:
    virtual void process(void);
  
  public:
    NullIRQHandler(sys_irq irq);
    ~NullIRQHandler();
};

void NullIRQHandler::process(){
}

NullIRQHandler::NullIRQHandler(sys_irq irq){
}

NullIRQHandler::~NullIRQHandler(){
}

*/

void nullirqhandler(void){
}


void irq_init(void){
  int i;
  for(i=0;i!=16;i++){
    g_irqHandlers[i] = nullirqhandler; /*  */
  }
  
  /* enable slave int. */
  outportb(0x21, inportb(0x21) & 0xfb); /* -> pic.cpp */

  /* MapDefaultHandlers */
  g_irqMap->mark(0); /* Timer */
  //g_irqMap->mark(1); /* KEY   */
  g_irqMap->mark(6); /* FDC   */
}

sys_irq irq_request(BitMap* irqmap){
  int i;
  for(i=0;i!=16;i++){
    if(irqmap->marked(i)){
      if(!g_irqMap->marked(i)){
        return i;
      }
    }
  }
  return IRQ_INVALID;
}

bool irq_acquire(sys_irq irq,IRQHandler ih){
  if(g_irqMap->marked(irq)){
    g_console->printf("irq:irq_acquire() failed.\n");
    return false;
  }
  g_console->printf("irq:installing IRQ Handler(%x).\n",(dword)ih);
  
  g_irqMap->mark(irq);
  g_console->printf("irq:dummy process().\n");
  ih();
  g_irqHandlers[irq] = ih;
  return true;
}

void irq_free(sys_irq irq){
  irq_disable(irq);
  g_irqMap->clear(irq);
  //delete g_irqHandlers[irq];
  return;
}

void irq_enable(sys_irq irq){
  byte i;
  i = 1;
  i = i << (irq & 7);
  i = 0xff - i;
  if(irq & 0x08){/* slave */
    outportb(0x21, inportb(0x21) & i); /* -> pic.cpp */
  }else{         /* master */
    outportb(0xA1, inportb(0x21) & i); /* -> pic.cpp */
  }
}

void irq_disable(sys_irq irq){
  byte i;
  i = 1;
  i = i << (irq & 7);
  if(irq & 0x08){/* slave */
    outportb(0x21, inportb(0x21) | i); /* -> pic.cpp */
  }else{         /* master */
    outportb(0xA1, inportb(0x21) | i); /* -> pic.cpp */
  }
}
