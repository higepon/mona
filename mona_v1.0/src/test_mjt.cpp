#include <IDEDriver.h>
#include <global.h>
#include <BitMap.h>
#include <test_mjt.h>
#include <sysresource.h>
#include <driver/ISADriver.h>

#include "driver/ISADevice/PS2KBC/ps2kbc.h"
#include "driver/ISADevice/PCIC/PCIC.h"
#include<pic.h>
//#include <expr_driver/module.h>
void test_mjt_init(){
  g_irqMap = new BitMap(16);
}
void test_sysresource(){

  ISADriver *isa;
  PS2KBC *m;
  PCIC *p;
  isa = new ISADriver(g_console);
  m = new PS2KBC(isa);
  p = new PCIC(isa);
}
void test_cmos(){
  dword b; /* byte */
  disableInterrupt();
  
  outportb(0x70,0x31);
  b = inportb(0x71);
  b = b /4 ;
  g_console->printf("reported EX.mem size = %d MB\n",b);
  enableInterrupt();
}
void test_ide(){
    /* IDE TEST routine */
    dword idet;
    //kthread_init();
    enableInterrupt();
    enableTimer();
    enableKeyboard();
    idet = g_kthreadInfo.tick;
    g_console->printf("Enable timer...(DISABLED THREAD SCHEDULER!)\n");/* ihandlers.cpp timer, kthread.cpp tick */
    g_console->printf("IDE init...\n");
    IDEDriver *d0;
    IDEDriver *d1;
    g_console->printf("Primary...\n");
    d0 = new IDEDriver(g_console,0x1f0);
    g_console->printf("Secondry...\n");
    d1 = new IDEDriver(g_console,0x170);
    g_console->printf("Disable timer...\n");
    disableTimer();
    idet = g_kthreadInfo.tick - idet;
    g_console->printf("IDE init done(%d ticks)\n",idet);
    /* ~IDE */
}

void test_mjt(void){
  test_mjt_init();
  test_ide();
  test_cmos();
  test_sysresource();
}

