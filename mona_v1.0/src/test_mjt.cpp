#include <IDEDriver.h>
#include <global.h>
#include <test_mjt.h>
#include<pic.h>
//#include <expr_driver/module.h>
void test_ide(){
    /* IDE TEST routine */
    dword idet;
    //kthread_init();
    enableInterrupt();
    enableTimer();
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
  test_ide();
  //test_loadmodule(romfs,"dummy.mod");
}

