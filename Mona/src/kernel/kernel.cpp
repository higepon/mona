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

#include <sys/types.h>
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
#include <string.h>
#include <syscalls.h>
#include <PageManager.h>
#include <elf.h>
#include <MemoryManager.h>
#include <vbe.h>
#include <VesaConsole.h>
#include <LogConsole.h>

char* version = "Mona version.0.2.0beta1 $Date$";
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

    g_log = new LogConsole();

    g_console->printf("%s\nCopyright (c) 2002-2004 higepon\n\n", version);

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
    g_console->printf("\nSystem Total Memory %d[MB]. VRAM=%x Paging on \n", g_total_system_memory / 1024 / 1024, g_vesaDetail->physBasePtr);

    /* shared memory object */
    SharedMemoryObject::setup();

    /* messenger */
    g_messenger = new Messenger(512);

    /* IDManager */
    g_id = new IDManager();

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
    g_scheduler->join(idleThread);

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

void loadServer(const char* server, const char* name)
{
    g_console->printf("loading %s....", server);
    g_console->printf("%s\n", loadProcess(server, name, true, NULL) ? "NG" : "OK");

    MessageInfo msg;

    for (;;)
    {
        if (g_messenger->receive(g_currentThread->thread, &msg)) continue;

        if (msg.header == MSG_SERVER_START_OK) break;
    }
    return;
}

int execSysConf()
{
    /* only one process can use fd */
    while (Semaphore::down(&g_semaphore_fd));

    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    /* file open */
    if (!(g_fs->open("/MONA.CFG", 1)))
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return 1;
    }

    /* get file size and allocate buffer */
    int fileSize  = g_fs->size();

    int readTimes = fileSize / 512 + (fileSize % 512 ? 1 : 0);
    byte* buf     = (byte*)malloc(512 * readTimes);

    memset(buf, 0, 512 * readTimes);
    if (buf == NULL)
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return 2;
    }

    /* read */
    if (!g_fs->read(buf, fileSize))
    {
        free(buf);
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return g_fs->getErrorNo();
    }

    /* close */
    if (!g_fs->close())
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
    }

    g_fdcdriver->motorAutoOff();
    Semaphore::up(&g_semaphore_fd);

    /* execute */
    char line[256];
    int linepos = 0;
    for (int pos = 0; pos <= fileSize; pos++) {
        char ch = pos < fileSize ? (char)buf[pos] : '\n';
        if (ch == '\r' || ch == '\n') {
            if (linepos > 0) {
                line[linepos] = '\0';
                if (strstr(line, "SERVER=") == line) {
                    const char* server = &line[7];
                    const char* name = &line[linepos - 1];
                    for (;; name--) {
                        if (*name == '/') {
                            name++;
                            break;
                        }
                        if (name == server) break;
                    }
                    loadServer(server, name);
                }
                linepos = 0;
            }
        } else if (linepos < 255) {
            line[linepos++] = ch;
        }
    }

    free(buf);
    return 0;
}

void mainProcess()
{
    /* FDC do not delete */
    enableFDC();
    g_fdcdriver = new FDCDriver();
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    g_fs = new FSOperation();

    if (g_fs == NULL || !(g_fs->initialize((IStorageDevice*)g_fdcdriver)))
    {
        g_console->printf("FSOperation::initialize error\n");
        for (;;);
    }

    g_fdcdriver->motorAutoOff();

    if (execSysConf() != 0)
    {
        g_console->printf("/MONA.CFG does not exist\n");
        for (;;);
    }

    enableKeyboard();
    enableMouse();

    /* end */
    int result;
    SYSCALL_0(SYSTEM_CALL_KILL, result);
    for (;;);
}
