/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002,2003,2004 Higepon
    Copyright (c) 2002,2003      Guripon
    Copyright (c) 2003           .mjt
    Copyright (c) 2004           Gaku

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both
    the above copyright notice(s) and this permission notice appear in supporting documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS
    NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
    WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written authorization of the copyright holder.
*/

/*!
    \file   kernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002,2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#define GLOBAL_VALUE_DEFINED

#include <types.h>
#include <global.h>
#include <kernel.h>
#include <operator.h>
#include <tester.h>
#include <checker.h>
#include <KeyBoardManager.h>
#include <FDCDriver.h>
#include <GraphicalConsole.h>
#include <ihandlers.h>
#include <pic.h>
#include <BitMap.h>
#include <FAT12.h>
#include <string.h>
#include <syscalls.h>
#include <PageManager.h>
#include <elf.h>
#include <MemoryManager.h>
#include <vbe.h>
#include <VesaConsole.h>

char* version = "Mona version.0.1.7 $Date$";
void  mainProcess();

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2003/06/08
*/
void startKernel(void)
{
    /* kernel memory range */
    km.initialize(0x200000, 0x7fffff);

    /* set segment */
    GDTUtil::setup();

    /* VESA */
    g_vesaInfo = new VesaInfo;
    memcpy(g_vesaInfo, (VesaInfo*)0x800, sizeof(VesaInfo));

    /* console */
    if (g_vesaInfo->sign[0] == 'N')
    {
        g_console = new GraphicalConsole();
        g_console->printf("VESA not supported. sorry kernel stop.\n");
        for (;;);
    }
    else
    {
        g_vesaDetail = new VesaInfoDetail;
        memcpy(g_vesaDetail, (VesaInfoDetail*)0x830, sizeof(VesaInfoDetail));
        g_console = new VesaConsole(g_vesaDetail);
        g_console->setCHColor(GP_LIGHTGREEN);
        g_console->setBGColor(GP_WHITE);
        g_console->clearScreen();
    }

    pic_init();
    RTC::init();
    printOK("Setting PIC        ");

    IDTUtil::setup();
    printOK("Setting IDT        ");
    printOK("Setting GDT        ");

    checkTypeSize();
    printOK("Checking type size ");

    /* mouse init */
    int mouse = Mouse::init();
    if (!mouse) printOK("Setting Mouse      ");
    else g_console->printf("Mouse init error=%d\n", mouse);

    /* get total system memory */
    g_total_system_memory = MemoryManager::getPhysicalMemorySize();
    g_console->printf("\nSystem TotalL Memory %d[MB]. VRAM=%x Paging on \n", g_total_system_memory / 1024 / 1024, g_vesaDetail->physBasePtr);

    /* shared memory object */
    SharedMemoryObject::setup();

    /* messenger */
    g_messenger = new Messenger(256);

    /* paging start */
    g_page_manager = new PageManager(g_total_system_memory);
    g_page_manager->setup((PhysicalAddress)(g_vesaDetail->physBasePtr));

    /* dummy thread struct */
    Thread* dummy1 = new Thread();
    Thread* dummy2 = new Thread();
    g_prevThread    = dummy1->tinfo;
    g_currentThread = dummy2->tinfo;

    /* this should be called, before timer enabled */
    ProcessOperation::initialize(g_page_manager);
    g_scheduler = new Scheduler();

    /* at first create idle process */
    Process* idleProcess = ProcessOperation::create(ProcessOperation::KERNEL_PROCESS, "IDLE");
    Thread* idleThread = ThreadOperation::create(idleProcess, (dword)monaIdle);
    g_scheduler->join(idleThread, 60);

    /* start up Process */
    Process* initProcess = ProcessOperation::create(ProcessOperation::KERNEL_PROCESS, "INIT");
    Thread*  initThread  = ThreadOperation::create(initProcess, (dword)mainProcess);
    g_scheduler->join(initThread);

    disableTimer();

    enableInterrupt();

    g_info_level = MSG;

    /* dummy thread struct */
    g_prevThread    = dummy1->tinfo;
    g_currentThread = dummy2->tinfo;
    g_prevThread->archinfo->cr3    = 1;
    g_currentThread->archinfo->cr3 = 2;

    enableTimer();

#ifdef HIGE

#endif

    for (;;);
}

/*!
    \brief  mona kernel panic

    kernel panic

    \author HigePon
    \date   create:2002/12/02 update:2003/03/01
*/
void panic(const char* msg)
{
    g_console->setCHColor(GP_RED);
    g_console->printf("kernel panic\nMessage:%s\n", msg);
    for (;;);
}

void checkMemoryAllocate(void* p, const char* msg)
{
    if (p != NULL) return;
    panic(msg);
}

/*!
    \brief print OK

    print "msg             [OK]"

    \param msg message
    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printOK(const char* msg)
{
    static int i = 0;

    if (i % 2) g_console->printf("   ");

    g_console->printf((char*)msg);
    g_console->printf("[");
    g_console->setCHColor(GP_LIGHTBLUE);
    g_console->printf("OK");
    g_console->setCHColor(GP_LIGHTGREEN);
    g_console->printf("]");

    if (i % 2) g_console->printf("\n");
    i++;
}

void mainProcess()
{
#if 0
    byte buf[512];

    KDate dt1;
    KDate dt2;

    outportb(0x21, 0xff);
    outportb(0xA1, 0xff);
    outportb(0x21, inportb(0x21) & 0xBF);

    RTC::getDate(&dt1);
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    for (int i = 0; i < 20; i++)
    {
        g_fdcdriver->read(1,buf);
        g_fdcdriver->read(2,buf);
        g_fdcdriver->read(3,buf);
        g_fdcdriver->read(4,buf);
        g_fdcdriver->read(5,buf);
        g_fdcdriver->read(6,buf);
        g_fdcdriver->read(7,buf);
        g_fdcdriver->read(8,buf);
        g_fdcdriver->read(9,buf);
        g_fdcdriver->read(10, buf);
    }
    RTC::getDate(&dt2);

    g_console->printf("\n%d/%d/%d %d:%d:%d\n", dt1.year, dt1.month, dt1.day, dt1.hour, dt1.min, dt1.sec);
    g_console->printf("%d/%d/%d %d:%d:%d\n", dt2.year, dt2.month, dt2.day, dt2.hour, dt2.min, dt2.sec);

#endif

    /* FDC do not delete */
    enableFDC();
    g_fdcdriver = new FDCDriver();
    g_fat12     = new FAT12((DiskDriver*)g_fdcdriver);
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    if (!g_fat12->initilize())
    {
        g_console->printf("FAT INIT ERROR %d\n", g_fat12->getErrorNo());
        for (;;);
    }

    g_fdcdriver->motorAutoOff();

    /* KEY Server */
    g_console->printf("loading KeyBoard Server....");
    g_console->printf("%s\n", loadProcess("SERVER", "KEYBDMNG.SVR", true, NULL) ? "NG" : "OK");

    /* Mouse Server */
    g_console->printf("loading Mouse    Server....");
    g_console->printf("%s\n", loadProcess("SERVER", "MOUSE.SVR", true, NULL) ? "NG" : "OK");

    /* Shell Server */
    g_console->printf("loading Shell    Server....");
    g_console->printf("%s\n", loadProcess("SERVER", "SHELL.SVR", true, NULL) ? "NG" : "OK");
    enableKeyboard();
    enableMouse();

    /* end */
    ThreadOperation::kill();
    for (;;);
}
