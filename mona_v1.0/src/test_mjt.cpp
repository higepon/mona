#include <IDEDriver.h>
#include <global.h>
#include <string.h>
#include <BitMap.h>
#include <test_mjt.h>
#include <sysresource.h>
#include <driver/ISADriver.h>
#include <Module.h>
#include "driver/ISADevice/PS2KBC/ps2kbc.h"
#include "driver/ISADevice/PCIC/PCIC.h"
#include <pic.h>

void pn(char *p){
	strcpy(g_process_name,p);
}

void test_mjt_init(){
  g_irqMap = new BitMap(16);
}

Globals globals;

void test_sysresource(){
	
  ISADriver *isa;
  PS2KBC *m;
  PCIC *p;
	globals.debug = g_console;
	globals.info = g_console;
	globals.error = g_console;
  isa = new ISADriver(g_console);
  m = new PS2KBC(isa,&globals);
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
		pn("IDE");
    d0 = new IDEDriver(g_console,0x1f0);
    pn("(test_mjt)");
    g_console->printf("Secondry...\n");
    pn("IDE");
    d1 = new IDEDriver(g_console,0x170);
    pn("(test_mjt)");
    g_console->printf("Disable timer...\n");
    disableTimer();
    idet = g_kthreadInfo.tick - idet;
    g_console->printf("IDE init done(%d ticks)\n",idet);
    /* ~IDE */
}

void test_mjt(void){
	int d;
	d = g_info_level;
	g_info_level = 9999;
  pn("(test_mjt)");
  test_mjt_init();
  test_ide();
  test_cmos();
 // test_sysresource();
	pn("KERNEL");
	g_info_level = d;
  
}

